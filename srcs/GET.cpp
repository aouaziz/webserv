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
	this->Request_header["stop_receiving"] = "false";
}

HTTPMethod::~HTTPMethod()
{
}

HTTPMethod::HTTPMethod(const HTTPMethod &src)
{
	*this = src;
}

HTTPMethod &HTTPMethod::operator=(const HTTPMethod &rhs)
{
	if (this != &rhs)
	{
		this->htype = rhs.htype;
		this->_config = rhs._config;
		this->_Location_Scoop = rhs._Location_Scoop;
		this->_linker = rhs._linker;

		this->Path = rhs.Path;
		this->Uri = rhs.Uri;
		this->query = rhs.query;
		this->Method = rhs.Method;
		this->Version = rhs.Version;
		this->Request_header = rhs.Request_header;
		this->Response = rhs.Response;
		this->HeaderState = rhs.HeaderState;
		this->response_ready = rhs.response_ready;
		this->client_socket = rhs.client_socket;
		this->boundary = rhs.boundary;
		this->BodyLength = rhs.BodyLength;
		this->MimeType = rhs.MimeType;
		this->requested_file_fd = rhs.requested_file_fd;
		this->is_header_sent = rhs.is_header_sent;
		this->to_erase = rhs.to_erase;
	}
	return *this;
}


void HTTPMethod::serveIndexFile(int &indexFound)
{
	std::string indexPath = Path + '/' + _Location_Scoop.index;
	Path = indexPath;
	std::ifstream indexFile(indexPath.c_str());
	if (indexFile)
	{
		indexFound = 1;
		std::stringstream indexContent;
		indexContent << indexFile.rdbuf(); // rdbuf reads the entire indexFile
		indexFile.close();

		Request_header["body"] = indexContent.str();

		std::string extension = Path.substr(Path.find_last_of('.'));
		std::string mimeType = _linker.Mime_types[extension];
		if (mimeType.empty())
			mimeType = "text/plain";
		if (indexContent.str().empty())
			Request_header["content-length"] = "0";

		Request_header["content-type"] = mimeType;
		Request_header["content-length"] = to_string(indexContent.str().length());

		if (HeaderState == 0)
		{
			composeResponseHeader("200", mimeType, "", indexContent.str().length());
		}
	}
	else
		sendCodeResponse("404"); // Error: Index file not found
}

void HTTPMethod::serveAutoIndex(const std::string &directoryPath)
{
	DIR *directoryPtr;
	struct dirent *directoryEntry;
	directoryPtr = opendir(Path.c_str());

	if (directoryPtr == NULL)
	{
		sendCodeResponse("500"); // Error: Unable to retrieve file/directory information
		return;
	}

	std::string fullDirectoryPath = directoryPath;
	if (fullDirectoryPath[fullDirectoryPath.length() - 1] != '/')
	{
		fullDirectoryPath += '/';
	}

	std::string statusMessage = _linker.Status_codes_error["200"];
	std::ostringstream ResponseDir;
	ResponseDir << "<html><head><title>Directory list</title><style>body{font-family:Arial,sans-serif;background-color:#f4f4f4;margin:0;padding:0}h1{color:#333;text-align:center;margin-top:20px}ul{list-style-type:none;padding:0;margin:0}li{padding:10px;background-color:#fff;border-bottom:1px solid #ddd}li:hover{background-color:#f9f9f9}a{color:#007bff;text-decoration:none}a:hover{text-decoration:underline}</style></head><body><h1>Directory list</h1><ul>";
	std::string directoryList;
	while ((directoryEntry = readdir(directoryPtr)) != NULL) // read the directory
	{
		directoryList = fullDirectoryPath + directoryEntry->d_name;

		if (std::string(directoryEntry->d_name) != "..")
			ResponseDir << "<li><a href=\"" << directoryList << "\">" << directoryList.substr(directoryList.find_last_of("/") + 1) << "</a></li>";
	}
	closedir(directoryPtr);

	ResponseDir << "</ul></h1></body></html>";
	Request_header["Content-Type"] = "text/html";
	Request_header["Content-Length"] = to_string(ResponseDir.str().length());
	Request_header["body"] = ResponseDir.str();
	composeResponseHeader("200", ".html", "", ResponseDir.str().length());
}

void HTTPMethod::handleRequestedPath(int &IndexFound)
{
	std::string directoryPath = Uri;
	struct stat pathStat;

	if ((stat(Path.c_str(), &pathStat)) == 0) // if file found
	{
		if (pathStat.st_mode & S_IFDIR || pathStat.st_mode & S_IFREG) // if directory or regular file
		{
			if (directoryPath[directoryPath.length() - 1] != '/')
				directoryPath += '/';
			else if (directoryPath[directoryPath.length() - 1] == '/' && directoryPath.length() > 1)
				directoryPath = directoryPath.substr(0, directoryPath.length() - 1);
		}
	}
	if (_Location_Scoop.index.empty() == 0)	 // if index file is set
		serveIndexFile(IndexFound);			 // handle index file
	else if (_Location_Scoop.autoindex == 1) // if autoindex is enabled
		serveAutoIndex(directoryPath);		 // handle autoindex
	else
		sendCodeResponse("403"); // Error: No index file and autoindex is disabled
}

void HTTPMethod::handleFileRequest(Client &client)
{
	this->requested_file_fd = open(Path.c_str(), O_RDONLY);
	if (this->requested_file_fd == -1)
	{
		sendCodeResponse("404");
		return;
	}

	struct stat fileInfo;
	if (stat(Path.c_str(), &fileInfo) == -1)
	{
		sendCodeResponse("500");
		close(this->requested_file_fd);
		return;
	}

	std::string file_size = to_string(fileInfo.st_size);
	std::string extension = Path.substr(Path.find_last_of('.'));
	std::string mime_type = getMimeType(extension);

	Request_header["content-length"] = file_size;
	Request_header["content-type"] = mime_type;
	Request_header["query"] = this->query;
	Request_header["method"] = this->Method;
	HeaderState = 0;
	if (this->_Location_Scoop.cgi == true)
	{
		Cgi cgi(this->_Location_Scoop, Request_header);
		int res = cgi.execute_cgi(client, Path, extension);
		if (res == 0)
		{
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
	composeResponseHeader("200", mime_type, "", to_number(file_size.c_str()));
}

int HTTPMethod::GET(Client &client)
{
	int IndexFound = 0;
	struct stat CheckStat;
	if (!client.file_name.empty())
	{
		if (client.file_body)
			client.file_body->close();
		unlink(client.file_name.c_str());
		client.file_name = "";
	}
	if (stat(Path.c_str(), &CheckStat) == 0) // if file found
	{
		if (CheckStat.st_mode & S_IFDIR)	  // if directory
			handleRequestedPath(IndexFound);  // handle directory
		else if (CheckStat.st_mode & S_IFREG) // if regular file
			handleFileRequest(client);		  // handle file
	}
	else
		sendCodeResponse("404"); // Error: File not found
	return 0;
}
