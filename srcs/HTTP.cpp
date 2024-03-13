#include "../includes/HTTP.hpp"

HTTP::HTTP(ServerConfig serverConfig)
{
	this->_config = serverConfig;
	this->response_ready = false;
	valid_header = false;
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

int HTTP::prepapreHeaders(Client *client)
{
	std::string extension = Path.substr(Path.find_last_of('.'));
	// std::string mime_type = getMimeType(extension);
	Request_header["ext"] = extension;
	Request_header["query"] = this->query;
	Request_header["method"] = this->Method;
	Request_header["body_file"] = client->file_name;

	return 2;
}