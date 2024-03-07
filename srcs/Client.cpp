#include "../includes/Client.hpp"

Client::Client(ServerConfig serverConfig)
{
    this->request = new HTTP(serverConfig);
    requestHeader = "";
    finish_response = false;
    finish_request = false;
    finish_header = false;
    response_ready = false;
    addrlen = sizeof(new_addr);
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
        this->request = new HTTP(src.request->_config);
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
        this->file_name = src.file_name;
        this->cliantime = src.cliantime;
    }
    return *this;
}

void Client::ResetClient()
{
    finish_response = false;
    response_ready = false;
    finish_request = false;
    finish_header = false; 
    requestHeader = "";
    requestBody = "";
    response.clear();
    request->ResetMethod();
    BodySize = 0;
    if (file_body.is_open())
        file_body.close();
}
void Client::checkBody()
{
    BodySize += requestBody.size();
    if (BodySize > to_namber(request->_Location_Scoop.client_max_body_size.c_str()))
    {
        request->sendCodeResponse("413"); // Entity too large
        file_body.close();
        unlink(file_name.c_str());
        finish_request = true;
        request->response_ready = true;
        request->Request_header["stop_receiving"] = "true";
        return ;
    }
    file_body << requestBody;
    if (file_body.fail())
    {
        std::cerr << "Error: failed to write to file " << std::endl;
        request->sendCodeResponse("500");
        finish_request = true;
        request->response_ready = true;
        request->Request_header["stop_receiving"] = "true";
        return;
    }
    if (request->htype == ContentLength && BodySize == request->BodyLength)
    {
        finish_request = true;
    }
    else
    {
        size_t endOfBody = requestBody.find("\r\n0\r\n\r\n");
        if (endOfBody != std::string::npos && endOfBody + 7 == requestBody.size())
        {
            std::cout << "BODY SIZE " << BodySize << std::endl;
            finish_request = true;
        }
    }
}

void Client::CheckRequest2(std::string message)
{
    if (!finish_header)
    {
        finish_header = true;
        size_t endOfHeaders = message.find("\r\n\r\n");
        requestBody = message.substr(endOfHeaders + 4);
        requestHeader = message.substr(0, endOfHeaders);
        finish_request = request->parseRequestHeader(requestHeader);
        if (finish_request)
            return;

        create_file(request->Request_header);
    }
    else
        requestBody = message;

    checkBody();
}

int Client::ReceivesRequest()
{
    int clientfd = clien_socket;
    std::vector<char> buffer(BUFSIZE);
    int bytesRead = recv(clientfd, buffer.data(), buffer.size(), 0);
    if(handelSendAndRecv(Recv, bytesRead))
            return -1;
    cliantime = time(NULL);
    std::string header(buffer.begin(), buffer.begin() + bytesRead);
    header.resize(bytesRead);
    CheckRequest2(header);

    if (finish_request && !request->response_ready)
    {
        if (handleRedirections())
            return 0;
        if (request->Method == "GET")
            return request->GET();
        else if (request->Method == "POST")
            return request->POST(this);
        else if (request->Method == "DELETE")
        {
            request->DELETE(request->Path.c_str());
            return 0;
        }
    }
    return 0;
}
bool Client::handelSendAndRecv(int type, int namber)
{
  if(namber == 0)
  {
    std::cerr << "Connection closed by peer." << std::endl;
    return true;
  }
  else if(namber == 1)
  {
    std::cerr <<"server can't " << ((type == Send) ? "send to " : "recv from " )<< "client \n" ;
    return true;
  }
  return false;
}

int Client::handleRedirections()
{
    if (request->_Location_Scoop.redir.first != -1)
    {
        std::string code = to_string(request->_Location_Scoop.redir.first);
        std::string url = request->_Location_Scoop.redir.second;
        if (!request->_Location_Scoop.redir.second.empty())
        {
            request->Response = "HTTP/1.1 " + code + " " + request->_linker.Status_codes_error[code] + "\r\n";
            request->Response.append("Location: ");
            request->Response.append(url + "\r\n");
            request->Response.append("\r\n");
            finish_response = true;
        }
        else
            request->sendCodeResponse(code);
        return 1;
    }
    return 0;
}


// fill the body, and check if the request is finished

int Client::HandleResponse()
{
    int bytesSent;
    if (!request->is_header_sent)
    {
        bytesSent = send(clien_socket, request->Response.c_str(), request->Response.size(), 0);
        if(handelSendAndRecv(Send, bytesSent))
            return -1;
        request->is_header_sent = true;
        if (request->requested_file_fd == -1) // requested_file_fd is -1 when there is no body to send
            finish_response = true;
        return 0;
    }
    char buff[BUFSIZE];
    int bytesread = read(request->requested_file_fd, buff, BUFSIZE);
    if (bytesread < 0)
    {
        std::cerr << "Failed to read from file -> " << request->requested_file_fd << std::endl;
        return 1;
    }
    else if (bytesread == 0)
    {
        close(request->requested_file_fd);
        request->requested_file_fd = -1;
        request->is_header_sent = false;
        finish_response = true;
        return 0;
    }
    bytesSent = send(clien_socket, buff, bytesread, 0);
    if(handelSendAndRecv(Send, bytesSent))
            return -1;
    return (0);
}
