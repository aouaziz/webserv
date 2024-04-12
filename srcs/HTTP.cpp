#include "../includes/HTTP.hpp"

HTTP::HTTP(ServerConfig serverConfig)
{
	this->_config = serverConfig;
	this->response_ready = false;
	this->_linker = Linker();
	BodyLength = 0;
	htype = 0;
	this->requested_file_fd = -1;
	this->is_header_sent = false;
}

HTTP::~HTTP()
{
}

void HTTP::ResetMethod()
{
    response_ready = false;
    BodyLength = 0;
    Path = "";
    Version = "";
    Request_header.clear();
    Response.clear();
    Url = "";
    htype = 0;
    MimeType = "";
    requested_file_fd = -1;
    is_header_sent = false;
}



void HTTP::handleDirectoryRequest(int &IndexFound)
{
    std::cout << "URL : " << Url << '\n';
	if (Url[Url.length() - 1] != '/')
    {
		Url += '/';
        sendCodeResponse("301");
        return;
    }
	if (!_Location_Scoop.index.empty())	 // if index file is set
		handleIndexFileRequest(IndexFound);	 // handle index file
	else if (_Location_Scoop.autoindex == 1) // if autoindex is enabled
		handleAutoIndexRequest(Url); // handle autoindex
	else
		sendCodeResponse("403"); // Error: No index file and autoindex is disabled
}


// void HTTP::handleDirectoryRequest(int &IndexFound)
// {
// 	std::string IsDirectory = Url;
// 	struct stat CheckStat;

// 	if ((stat(Path.c_str(), &CheckStat)) == 0) // if file found
// 	{
// 		if (CheckStat.st_mode & S_IFDIR) // if directory
// 		{
// 			if (IsDirectory[IsDirectory.length() - 1] != '/')
// 				IsDirectory += '/';
// 			else if (IsDirectory[IsDirectory.length() - 1] == '/' && IsDirectory.length() > 1)
// 				IsDirectory = IsDirectory.substr(0, IsDirectory.length() - 1);
// 		}
// 		else if (CheckStat.st_mode & S_IFREG) // if regular file
// 		{
// 			if (IsDirectory[IsDirectory.length() - 1] != '/')
// 				IsDirectory += '/';
// 			else if (IsDirectory[IsDirectory.length() - 1] == '/' && IsDirectory.length() > 1)
// 				IsDirectory = IsDirectory.substr(0, IsDirectory.length() - 1);
// 		}
// 	}
// 	if (_Location_Scoop.index.empty() == 0)	 // if index file is set
// 		handleIndexFileRequest(IndexFound);	 // handle index file
// 	else if (_Location_Scoop.autoindex == 1) // if autoindex is enabled
// 		handleAutoIndexRequest(IsDirectory); // handle autoindex
// 	else
// 		sendCodeResponse("403"); // Error: No index file and autoindex is disabled
// }