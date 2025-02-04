#include "../includes/Client.hpp"

Client::Client(ServerConfig serverConfig)
{
    this->request = new HTTPMethod(serverConfig);
    requestHeader = "";
    finish_response = false;
    finish_request = false;
    finish_header = false;
    BodySize = 0;
    is_cgi_running = false;
    cgi_pid = -1;

    has_chunked_body = false;
    chunk_size = 0;
    file_body = NULL;
    chunk_output = NULL;
}

Client::~Client()
{
    // if (this->file_body)
    // {
    //     if (this->file_body->is_open())
    //         this->file_body->close();
    // }

    delete this->request;
}

void Client::create_file(MapOf_Str_Str &Request_header)
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
    if (file_body)
    {
        delete file_body;
    }
    file_body = new std::fstream(file_name.c_str(), std::ios::out | std::ios::in | std::ios::binary | std::ios::app);
    if (!file_body || !file_body->is_open())
    {
        std::cerr << "Error: Can't open the file" << std::endl;
    }
}
void Client::delete_file()
{
    if (!this->file_body)
        return;
    if (this->file_body->is_open())
    {
        this->file_body->close();
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
        this->request = new HTTPMethod(*src.request);
        this->response = src.response;
        this->finish_response = src.finish_response;
        this->finish_request = src.finish_request;
        this->finish_header = src.finish_header;

        this->file_body = src.file_body;
        this->BodySize = src.BodySize;
        this->client_socket = src.client_socket;
        this->requestHeader = src.requestHeader;
        this->requestBody = src.requestBody;
        this->file_name = src.file_name;
        this->clientime = src.clientime;
        this->cgi_pid = src.cgi_pid;
        this->is_cgi_running = src.is_cgi_running;
        this->cgi_start_time.tv_sec = src.cgi_start_time.tv_sec;
        this->cgi_start_time.tv_usec = src.cgi_start_time.tv_usec;

        this->chunk_size = src.chunk_size;
        this->has_chunked_body = src.has_chunked_body;
        this->chunk_data = src.chunk_data;
        this->chunk_file_name = src.chunk_file_name;
        this->chunk_output = src.chunk_output;
    }
    return *this;
}
