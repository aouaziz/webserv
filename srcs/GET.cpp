#include "../includes/HTTPMethod.hpp"

HTTPMethod::HTTPMethod(ServerConfig serverConfig)
{
	this->_config = serverConfig;
	this->response_ready = false;
	this->_linker = Linker();
	HeaderState = 0;
	BodyLength = 0;
	htype = 0;
	this->requested_file_fd = -1;
	this->is_header_sent = false;
}

HTTPMethod::~HTTPMethod()
{
}

void HTTPMethod::replaceCarriageReturn(std::string &inputString)
{
	size_t found = inputString.find('\r');

	while (found != std::string::npos)
	{ // replace all '\r' with ' '
		inputString.erase(found, 1);
		found = inputString.find('\r', found + 1);
	}
}

// SETTERS
void HTTPMethod::setConfig(ServerConfig &config)
{
	this->_config = config;
}

void HTTPMethod::setData()
{
	this->HeaderState = 0;
	// post atributs
	this->AmountRecv = 0;
	this->PostFd = 0;
	this->PostPath = "";
	this->MimeType = "";
	this->HexaChunkStatus = 0;
	this->ChunkSize = 0;
	this->ChunkHexStr = "";
	// this->BodyLength = 0;
}

// GETERS
std::string HTTPMethod::GetRoot(const std::string &uri, const std::string &locationPath, const std::string &root)
{
	std::string matchedUri = uri;
	std::size_t locationPos = matchedUri.find(locationPath);
	if (locationPos != std::string::npos)
		matchedUri.replace(locationPos, locationPath.length(), root + "/");
	return matchedUri;
}

bool compare(std::string &Path, LocationConfig &location)
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

std::string HTTPMethod::getMimeType(const std::string &extension)
{
	if (_linker.Mime_types.find(extension) != _linker.Mime_types.end())
		return _linker.Mime_types[extension];
	else
		return "text/plain";
}

void HTTPMethod::CheckRecvFlags()
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
	else if ((it = Request_header.find("Content-Type")) != Request_header.end() && it->second.find("multipart/form-data") != std::string::npos)
	{
		htype = MultipartFormData;
		size_t semiPos = it->second.find(';');
		if (semiPos != std::string::npos)
		{
			boundary = it->second.substr(semiPos + 1);
			boundary.append("--");
		}
	}
	else
		htype = Unknown;
}

int HTTPMethod::parseRequestHeader(std::string req)
{
	std::vector<std::string> lines;
	std::istringstream iss(req);
	std::string line;
	while (std::getline(iss, line))
	{
		replaceCarriageReturn(line);
		lines.push_back(line);
	}

	std::istringstream first_line_iss(lines[0]);

	first_line_iss >> this->Method >> this->Uri >> this->Version;
	size_t pos = this->Uri.find("?");
	if (pos != std::string::npos)
	{
		this->query = this->Uri.substr(pos + 1);
		this->Uri = this->Uri.substr(0, pos);
	}
	for (size_t i = 1; i < lines.size(); i++)
	{
		std::string header_line = lines[i];
		size_t colon_pos = header_line.find(':');
		if (colon_pos != std::string::npos)
		{
			std::string key = header_line.substr(0, colon_pos);
			std::string value = header_line.substr(colon_pos + 2); // +2 to skip ':' and space
			Request_header.insert(std::make_pair(key, value));
		}
	}
	CheckRecvFlags();
	if (this->PrepAndValidateRequest() || this->Method == "GET" || this->Method == "DELETE")
		return 1;
	return 0;
}

bool fn(LocationConfig &x, LocationConfig &y)
{
	return (x.path.length() > y.path.length());
}

bool HTTPMethod::PrepAndValidateRequest()
{
	std::string uri = this->Uri;
	const std::string NonWantedChar = " <>{}|\\^`";

	if (uri.empty())
	{
		sendCodeResponse("400");
		return true;
	}
	if (Uri.find("..") != std::string::npos)
	{
		sendCodeResponse("403");
		return true;
	}
	if (Uri.length() > 2048)
	{
		sendCodeResponse("414");
		return true;
	}
	if (uri.find_first_of(NonWantedChar) != std::string::npos)
	{
		sendCodeResponse("400");
		return true;
	}

	if (Method != "GET" && Method != "POST" && Method != "DELETE")
	{
		sendCodeResponse("501");
		return true;
	}
	if (Version != "HTTP/1.1")
	{
		sendCodeResponse("505");
		return true;
	}
	if (this->Method == "POST")
	{
		if (this->htype == ContentLength && this->BodyLength == 0)
		{
			sendCodeResponse("400");
			return true;
		}
		else if (this->htype == Unknown)
		{
			sendCodeResponse("411");
			return true;
		}
	}
	this->Path = this->Uri;
	std::vector<LocationConfig> serverLocations = this->_config.locations;
	size_t i = 0;
	if (this->Uri.find('?') != std::string::npos)
		Path = this->Uri.substr(0, this->Uri.find('?'));
	std::sort(serverLocations.begin(), serverLocations.end(), fn);
	for (i = 0; i < serverLocations.size(); i++)
	{
		if (compare(Path, serverLocations[i]))
		{
			this->_Location_Scoop = serverLocations[i];
			break;
		}
	}
	if (i == serverLocations.size())
	{
		this->sendCodeResponse("404");
		return true;
	}
	std::vector<std::string> methods_of_location = this->_Location_Scoop.methods;
	std::vector<std::string>::iterator it;
	bool methodFound = false;

	for (it = methods_of_location.begin(); it != methods_of_location.end(); ++it)
	{
		if (*it == this->Method)
		{
			methodFound = true;
			break;
		}
	}
	if (!methodFound)
	{
		std::cout << "Method not allowed2" << std::endl;
		this->sendCodeResponse("405");
		return true;
	}
	Path = GetRoot(this->Path, this->_Location_Scoop.path, this->_Location_Scoop.root);
	return false;
}

bool isMethodAllowed(const std::vector<std::string> &allowedMethods, const std::string &method)
{
	return (std::find(allowedMethods.begin(), allowedMethods.end(), method) != allowedMethods.end());
}

LocationConfig findtLocation(const std::vector<LocationConfig> &serverLocations, const std::string &Path)
{
	LocationConfig _Location_Scoop;
	for (size_t i = 0; i < serverLocations.size(); i++)
	{
		size_t j = 0;
		while (j < Path.size() && j < serverLocations[i].path.size() && Path[j] == serverLocations[i].path[j])
			j++;
		if ((j == Path.size() && j == serverLocations[i].path.size()) || (j == serverLocations[i].path.size() && ((j < Path.size() && Path[j] == '/') || (j > 0 && Path[j - 1] == '/'))))
		{
			_Location_Scoop = serverLocations[i];
			break;
		}
	}
	return _Location_Scoop;
}

bool HTTPMethod::MatchLocation()
{
	this->Path = this->Uri;
	std::vector<LocationConfig> serverLocations = this->_config.locations;
	size_t i = 0;
	if (this->Uri.find('?') != std::string::npos)
		Path = this->Uri.substr(0, this->Uri.find('?'));
	this->_Location_Scoop = findtLocation(serverLocations, Path);

	if (i == serverLocations.size())
	{
		this->sendCodeResponse("404");
	}
	if (!isMethodAllowed(this->_Location_Scoop.methods, this->Method))
	{
		std::cout << "Method not allowed" << std::endl;
		this->sendCodeResponse("405");
		return 0;
	}
	Path = GetRoot(this->Path, this->_Location_Scoop.path, this->_Location_Scoop.root); // get the right path for the file requested
	return 1;
}

// SEND RESPONSE HEADER
std::string HTTPMethod::GenerateDirectoryList(std::string statusCode, std::string ls)
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

void HTTPMethod::handleDirectoryRequest(int &IndexFound)
{
	std::string IsDirectory = Uri;
	struct stat CheckStat;

	if ((stat(Path.c_str(), &CheckStat)) == 0) // if file found
	{
		if (CheckStat.st_mode & S_IFDIR) // if directory
		{
			if (IsDirectory[IsDirectory.length() - 1] != '/')
				IsDirectory += '/';
			else if (IsDirectory[IsDirectory.length() - 1] == '/' && IsDirectory.length() > 1)
				IsDirectory = IsDirectory.substr(0, IsDirectory.length() - 1);
		}
		else if (CheckStat.st_mode & S_IFREG) // if regular file
		{
			if (IsDirectory[IsDirectory.length() - 1] != '/')
				IsDirectory += '/';
			else if (IsDirectory[IsDirectory.length() - 1] == '/' && IsDirectory.length() > 1)
				IsDirectory = IsDirectory.substr(0, IsDirectory.length() - 1);
		}
	}
	if (_Location_Scoop.index.empty() == 0)	 // if index file is set
		handleIndexFileRequest(IndexFound);	 // handle index file
	else if (_Location_Scoop.autoindex == 1) // if autoindex is enabled
		handleAutoIndexRequest(IsDirectory); // handle autoindex
	else
		sendCodeResponse("403"); // Error: No index file and autoindex is disabled
}

void HTTPMethod::handleIndexFileRequest(int &IndexFound)
{
	std::string indexfilepath;
	indexfilepath = Path + '/' + _Location_Scoop.index;
	Path = indexfilepath;
	IndexFound = 1;
	std::ifstream file(indexfilepath.c_str());
	if (file)
		handleIndexFileExistence(file);
	else
		sendCodeResponse("404"); // Error: Index file not found
}

void HTTPMethod::handleAutoIndexRequest(const std::string &IfDir)
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


void HTTPMethod::handleFileRequest()
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
	HeaderState = 0;
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

void HTTPMethod::handleIndexFileExistence(std::ifstream &file)
{
	std::stringstream req_data;
	req_data << file.rdbuf();
	file.close();
	std::string extension = Path.substr(Path.find_last_of('.'));
	std::string mime_type = _linker.Mime_types[extension];
	Request_header["body"] = req_data.str();
	if (req_data.str().empty())
		Request_header["content-length"] = "0";
	Request_header["content-length"] = req_data.str().length();
	if (mime_type.empty())
		mime_type = "text/plain";
	Request_header["content-type"] = mime_type;
	if (HeaderState == 0)
		SendResponseHeader("200", mime_type, "", req_data.str().length());
}

int HTTPMethod::GET()
{
	int IndexFound = 0;
	struct stat CheckStat;
	if (stat(Path.c_str(), &CheckStat) == 0) // if file found
	{
		if (CheckStat.st_mode & S_IFDIR)		// if directory
			handleDirectoryRequest(IndexFound); // handle directory
		else if (CheckStat.st_mode & S_IFREG)	// if regular file
			handleFileRequest();				// handle file
	}
	else
	{
		sendCodeResponse("404"); // Error: File not found
	}
	return 0;
}
