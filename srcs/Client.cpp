#include "../includes/Client.hpp"

Client::Client(ServerConfig serverConfig)
{
    this->request = new HTTP(serverConfig);
    requestHeader = "";
    finish_response = false;
    finish_request = false;
    finish_header = false;
    ConnectionClose = false;
    size_of_chunk = 0;
    response_ready = false;
    to_erase = false;
    chunk_remaining ="";
    addrlen = sizeof(new_addr);
    BodySize = 0;

}

Client::~Client()
{
    if (this->file_body.is_open())
        this->file_body.close();
    delete this->request;
}

void	Client::GenerateRequestFile(MapOf_Str_Str	&Request_header)// need to be re write 
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
    ConnectionClose = false; 
    to_erase = false;
    size_of_chunk = 0;
    requestHeader = "";
    requestBody = "";
    chunk_remaining ="";
    response.clear();
    request->ResetMethod();
    BodySize = 0;
    if (file_body.is_open())
        file_body.close();
}
bool Client::HandleClientMaxBodySize()
{
    BodySize += requestBody.size();
    if (BodySize > to_namber(request->_Location_Scoop.client_max_body_size.c_str())) // 
    {
        request->sendCodeResponse("413"); // Entity too large
        file_body.close();
        unlink(file_name.c_str());
        finish_request = true;
        request->response_ready = true;
        request->requested_file_fd = -1;      
        return true;
    }
    return false;
}

bool Client::eraseProcessedData()
{
	size_t r = chunk_remaining.size() - size_of_chunk;
 	if (r >= 2)
	{
		chunk_remaining.erase(0, size_of_chunk + 2);
        to_erase =false;
    }
	else
	{
		chunk_remaining.erase(0, size_of_chunk);
		size_of_chunk = 0;
		to_erase = true;
        return true;
	}
    return false;
}

bool Client::processRemainingData()
{
	if (to_erase)
	{
		chunk_remaining.erase(0, 2);
		to_erase = false;
	}
	if (size_of_chunk > 0)
	{
		if (size_of_chunk > chunk_remaining.length())// we need to read moor
		{
			size_of_chunk -= chunk_remaining.length();
			file_body << chunk_remaining;
            chunk_remaining ="";

            return true;
		}

		file_body << chunk_remaining.substr(0, size_of_chunk);
		if(eraseProcessedData())
            return true;
	}
    return false;
}

void  Client::processChunkedData()
{
    std::string size ;
	size_t pos = chunk_remaining.find("\r\n");
	
    while (pos != std::string::npos)
	{
        size = chunk_remaining.substr(0,pos);
        size_of_chunk = FromHexToNamber(size);
		std::cout << size << " : " << size_of_chunk << std::endl;
		if (size_of_chunk == 0) // we reached the end of the body
        {
            finish_request = true;
			return;	
        }
		chunk_remaining.erase(0, size.length() + 2);//skip size + \r\n
		if (size_of_chunk <= chunk_remaining.size())
		{
			file_body << chunk_remaining.substr(0, size_of_chunk);
			if (eraseProcessedData())
            {
			    std::cout <<"chunk_remaining : " << chunk_remaining << '\n';
				return ;
            }
			pos = chunk_remaining.find("\r\n");	   // Find next chunk size
            std::cout << pos << " <= \n\n";
        }
		else// the chunk size was larger so we need to read again..
		{
			size_of_chunk -= chunk_remaining.length();
			file_body << chunk_remaining;
			chunk_remaining = "";
			break ;
		}
	}
    std::cout << "ana khrajt \n";
}


void Client::HandleChunkedPost()
{
    chunk_remaining.append(requestBody);
    if(processRemainingData())
    {
        return;
    }
    processChunkedData();
}


void Client::HandleContentLengthPost()
{
	file_body << requestBody;
    if (file_body.fail())
    {
        request->sendCodeResponse("500");
        finish_request = true;
        request->response_ready = true;
        request->requested_file_fd = -1;      
        return;
    }
    if (BodySize == request->BodyLength)
        finish_request = true;
}

void Client::ProcessAndValidateBody()
{
    if(HandleClientMaxBodySize())
        return;

    if(request->htype == Chunk)
        HandleChunkedPost();
    else 
        HandleContentLengthPost();   
}

void Client::HandleRequestBody(std::string message)
{
    if (!finish_header)
    {
        finish_header = true;
        size_t endOfHeaders = message.find("\r\n\r\n");
        requestBody = message.substr(endOfHeaders + 4);
        requestHeader = message.substr(0, endOfHeaders);
        finish_request = request->Request(requestHeader);
        if(request->Request_header["Connection"] == "close")
            ConnectionClose = true;
        if (finish_request)
            return;
        GenerateRequestFile(request->Request_header);
    }
    else
        requestBody = message;
    ProcessAndValidateBody();
}

int Client::ReceivesRequest()
{
    int clientfd = clien_socket;
    char buffer[BUFSIZE];
    int bytesRead = recv(clientfd, buffer, BUFSIZ, 0);
    if(handelSendAndRecv(Recv, bytesRead))
            return -1;
    cliantime = time(NULL);
    std::string header(buffer);
    header.resize(bytesRead);
    HandleRequestBody(header);
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
        if(ConnectionClose ==true )
            return -1;
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
