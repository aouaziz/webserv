   #include "../includes/HTTP.hpp"



bool HTTP::compare(std::string &Path, LocationConfig &location)
{
	size_t it = 0;
	size_t size_Path = Path.size();
	size_t size_location_path = location.path.size();
	while (it < size_Path && it < size_location_path)
	{
		if (Path[it] == location.path[it])
			it++;
		else
			break;
	}
	if (it == location.path.size())
	{
		if (it == Path.size() || (it < Path.size() && Path[it] == '/') || (it > 0 && Path[it - 1] == '/'))
			return true;
	}
	return false;
}

std::string HTTP::getMimeType(const std::string &extension)
{
	if (_linker.File_extensions.find(extension) != _linker.File_extensions.end())
		return _linker.File_extensions[extension];
	else
		return "text/plain";
}

void HTTP::RequestType()
{
	std::map<std::string, std::string>::iterator it;
	if ((it = Request_header.find("Transfer-Encoding")) != Request_header.end() && it->second.find("chunked") != std::string::npos)
	{
		htype = Chunk;
	}
	else if ((it = Request_header.find("Content-Length")) != Request_header.end())
	{
		htype = ContentLength;
		BodyLength = to_namber(it->second.c_str());
	}
	else
		htype = Unknown;
}



// SEND RESPONSE HEADER
std::string HTTP::GenerateDirectoryList(std::string statusCode, std::string ls)
{
	if (ls.empty())
	{
		std::cerr << "Error: GenerateDirectoryList <Directory listing>" << std::endl;
		sendCodeResponse("500");
		return "";
	}
	std::string statusMessage = _linker.Status_codes_error[statusCode];
	std::stringstream stream(ls);
	std::ostringstream ResponseDir;
	ResponseDir << "<html><head><title>Directory list</title></head><style>body { font-family: Arial, sans-serif; } ul { list-style-type: none; } a { color: blue; text-decoration: none; }</style><body><h1>Directory list</h1><ul>";
	std::string entry;
	while (std::getline(stream, entry, '\n'))
	{
		if (entry != "..")
			ResponseDir << "<li><a href=\"" << entry << "\">" << entry.substr(entry.find_last_of("/") + 1) << "</a></li>";
	}
	ResponseDir << "</ul></h1></body></html>";
	Request_header["Content-Type"] = "text/html";
	Request_header["Content-Length"] = to_string(ResponseDir.str().length());
	return ResponseDir.str();
}

// PROCCESS REQUEST

void HTTP::handleIndexFileRequest(int &IndexFound)
{
	std::string indexfilepath;
	indexfilepath = Path + _Location_Scoop.index;
	Path = indexfilepath;
	IndexFound = 1;
	std::ifstream file(indexfilepath.c_str());
	if (file)
		handleIndexFileExistence(file);
	else
		sendCodeResponse("404"); // Error: Index file not found
}

void HTTP::handleAutoIndexRequest(const std::string &IfDir)
{ // Done
	DIR *DirPtr;
	struct dirent *Dir;
	DirPtr = opendir(Path.c_str());
	if (DirPtr == NULL)
	{
		sendCodeResponse("500"); // Error: Unable to retrieve file/directory information
		return;
	}
	if (DirPtr)
	{
		std::string DirStr;
		while ((Dir = readdir(DirPtr)) != NULL)
		{
			DirStr += IfDir + Dir->d_name + '\n';
		}
		closedir(DirPtr);

		if (!DirStr.empty())
		{
			std::string ListDirectory = GenerateDirectoryList("200", DirStr);
			Request_header["body"] = ListDirectory;
			SendResponseHeader("200", ".html", "", ListDirectory.length());
		}
		else
		{
			sendCodeResponse("403"); // Error: No autoindex and no index file
		}
	}
}


void HTTP::handleFileRequest()
{
	struct stat st;
	std::string file_size;
	this->requested_file_fd = open(Path.c_str(), O_RDONLY);
	// std::cout << "fd: " << this->requested_file_fd << std::endl;
	if (this->requested_file_fd == -1)
	{
		sendCodeResponse("404");
		return ;
	}
	if (stat(Path.c_str(), &st) == 0)
		file_size = to_string(st.st_size);
	else
	{
		close(this->requested_file_fd);
		this->requested_file_fd = - 1;
		sendCodeResponse("500");
		return;
	}
	std::string extension = Path.substr(Path.find_last_of('.'));
	std::string mime_type = getMimeType(extension);
	std::stringstream req_data;
	Request_header["content-length"] = file_size;
	Request_header["content-type"] = mime_type;
	Request_header["query"] = this->query;
	Request_header["method"] = this->Method;
	if (this->_Location_Scoop.cgi == true)
	{
		Cgi cgi(this->_Location_Scoop, Request_header);
		int res = cgi.execute_cgi(Path, extension);
		if (res == 0)
		{
			Response.append(cgi.header);
			Response.append(cgi.body);
			response_ready = true;
			close(this->requested_file_fd);
			requested_file_fd = -1;
			return;
		}
		if (res != -1)
		{
			close(this->requested_file_fd);
			requested_file_fd = -1;
			sendCodeResponse(to_string((size_t)res));
			return;
		}
	}
	SendResponseHeader("200", mime_type, "", to_namber(file_size.c_str()));
}

void HTTP::handleIndexFileExistence(std::ifstream &file)
{
	std::stringstream req_data;
	req_data << file.rdbuf();
	file.close();
	std::string extension = Path.substr(Path.find_last_of('.'));
	std::string mime_type = _linker.File_extensions[extension];
	Request_header["body"] = req_data.str();
	if (req_data.str().empty())
		Request_header["content-length"] = "0";
	Request_header["content-length"] = req_data.str().length();
	if (mime_type.empty())
		mime_type = "text/plain";
	Request_header["content-type"] = mime_type;
	SendResponseHeader("200", mime_type, "", req_data.str().length());
}

int HTTP::GET()
{
	int IndexFound = 0;
	struct stat CheckStat;
	if (!stat(Path.c_str(), &CheckStat)) // if file found
	{
		if (CheckStat.st_mode & S_IFDIR)		// if directory
			handleDirectoryRequest(IndexFound); // handle directory
		else if (CheckStat.st_mode & S_IFREG)	// if regular file
			handleFileRequest();				// handle file
	}
	else
		sendCodeResponse("404"); // Error: File not found
	return 0;
}
