#include "../includes/HTTP.hpp"

HTTP::HTTP(ServerConfig serverConfig)
{
	this->_config = serverConfig;
	this->response_ready = false;
	valid_header = false;
	this->_linker = Linker();
	HeaderState = 0;
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
    HeaderState = 0;
    Path = "";
    Version = "";
    Request_header.clear();
    Response.clear();
    boundary = "";
    Url = "";
    htype = 0;
    ChunkHexStr = "";
    HexaChunkStatus = 0;
    ChunkSize = 0;
    PostFd = 0;
    AmountRecv = 0;
    PostPath = "";
    MimeType = "";
    requested_file_fd = -1;
    is_header_sent = false;
}



void HTTP::handleDirectoryRequest(int &IndexFound)
{
	std::string IsDirectory = Url;

	if (Url[Url.length() - 1] != '/')
    {
		Url += '/';
        sendCodeResponse("301");
        
    }
	if (!_Location_Scoop.index.empty())	 // if index file is set
		handleIndexFileRequest(IndexFound);	 // handle index file
	else if (_Location_Scoop.autoindex == 1) // if autoindex is enabled
		handleAutoIndexRequest(IsDirectory); // handle autoindex
	else
		sendCodeResponse("403"); // Error: No index file and autoindex is disabled
}