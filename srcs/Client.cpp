#include "../includes/Client.hpp"

Client::Client(ServerConfig serverConfig)
{
    this->request = new HTTPMethod(serverConfig);
    requestHeader = "";
    finish_response = false;
    finish_request = false;
    finish_header = false;
    response_ready = false;
    BodySize = 0;

}

Client::~Client()
{
    if (this->file_body.is_open())
        this->file_body.close();
    delete this->request;
}

void	Client::create_file(MapOf_Str_Str	&Request_header)
{
    std::string MimeType;
    Linker linker;
    if (Request_header.find("Content-Type") != Request_header.end())
	{
		size_t SecPosition = Request_header["Content-Type"].find(';'); // Find the position of the first ';'
		if (SecPosition != std::string::npos)
			MimeType = Request_header["Content-Type"].substr(0, SecPosition); // Get the MIME type
		else
			MimeType = Request_header["Content-Type"]; // Get the MIME type
	}
    std::string ext = linker.File_extensions[MimeType];
    std::string path = this->request->_Location_Scoop.root;
    if (path[path.length() - 1] != '/')
        path += "/";
    this->file_name = linker.RandomFileName(path, ext);
    this->file_body.open(this->file_name.c_str(), std::ios::out | std::ios::in | std::ios::binary | std::ios::app);
}

void    Client::delete_file()
{
    if (this->file_body.is_open())
    {
        this->file_body.close();
        remove(this->file_name.c_str());
    }
}


Client::Client(Client const &src) : request(NULL)
{
    *this = src;
}

Client &Client::operator=(Client const &src)
{
    if (this != &src)
    {
        delete this->request;
        this->request = new HTTPMethod(src.request->_config);
        this->response = src.response;
        this->finish_response = src.finish_response;
        this->finish_request = src.finish_request;
        this->finish_header = src.finish_header;
        this->response_ready = src.response_ready;
        this->BodySize = src.BodySize;
        this->clien_socket = src.clien_socket;
        this->requestHeader = src.requestHeader;
        this->requestBody = src.requestBody;
        this->chunkedData = src.chunkedData;
        // this->file_body = src.file_body; WE CANT COPY FSTREAM, NO NEED ANYWAY...
        this->file_name = src.file_name;
        this->cliantime = src.cliantime;
    }
    return *this;
}
