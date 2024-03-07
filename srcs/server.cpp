#include "../includes/Server.hpp"

Server::Server(const ServerConfig &serverConfig) : serverConfig(serverConfig)
{
}

void Server::ServInit()
{
    struct addrinfo address;
    port = serverConfig.port;
    ip = serverConfig.host;
    memset(&address, 0, sizeof(address)); // make sure the struct is empty
    address.ai_family = AF_INET;          // IPV4
    address.ai_socktype = SOCK_STREAM;    // TCP stream sockets
    address.ai_flags = AI_PASSIVE;
    if (getaddrinfo(ip.c_str(), port.c_str(), &address, &servinfo) != 0) // ensure that the structure is properly initiated and all information is added correctly
        check(-1, "getaddrinfo error");
}

void Server::CreatServer()
{
    ServInit();
    check((server_socket = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)), "Socket creation failed");
    check(fcntl(server_socket, F_SETFL, O_NONBLOCK), "fcntl failed"); // setting the server in a non-blocking state
    int opt = 1;
    check(setsockopt(server_socket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)), "SETSOCKOPT failed "); // fix bind error
    check(bind(server_socket, servinfo->ai_addr, servinfo->ai_addrlen), "bind failed");
    check(listen(server_socket, FD_SETSIZE), "listen failed");
    freeaddrinfo(servinfo);
}

bool Server::HandleSendRecvResult(int type, int numBytes, int clientfd) {

    if (numBytes == 0) {
        std::cerr << "Connection closed with client " << clientfd << '\n';
        return true;  
    } 
    else if (numBytes < 0) {
        std::cerr << "Failed to "<< (type == Send ? "sending to " : "receiving from ") <<  clientfd << '\n';
        return true;  
    }
  return false;
}

int Server::AcceptClient(fd_set &fd_read)
{
    if (Clients.size() > 1022)
    {
        std::cerr << "Error: Maximum number of clients (1024) reached. Cannot accept new connections." << std::endl;
        return -1;
    }
    struct sockaddr_in new_addr;
    int addrlen = sizeof(struct sockaddr_in);
    Client tmp(serverConfig);
    tmp.clien_socket = accept(server_socket, (struct sockaddr *)&new_addr, (socklen_t *)&addrlen);
    if (tmp.clien_socket == -1)
    {
        std::cerr << "unable to establish a connection to the clein on port" << this->port << std::endl;
        return -1;
    }
    check(fcntl(tmp.clien_socket, F_SETFL, O_NONBLOCK), "fcntl failed"); // setting the server in a non-blocking state
    tmp.cliantime = time(NULL);
    FD_SET(tmp.clien_socket, &fd_read);
    Clients.push_back(tmp);
    return (tmp.clien_socket);
}

void Server::ClearClient(fd_set &fd_read, fd_set &fd_write, std::vector<Client>::iterator &it)
{
    FD_CLR(it->clien_socket, &fd_read);
    FD_CLR(it->clien_socket, &fd_write);
    close(it->clien_socket);
    it = Clients.erase(it);
    maxfd--;
}
void Server::Swapfd(int fd, fd_set &se, fd_set &cl)
{
    FD_SET(fd, &se);
    FD_CLR(fd, &cl);
}
void Server::Reset(std::vector<Client>::iterator it)
{
    // std::cout << "reset\n";
    it->finish_response = false;
    it->response_ready = false;
    it->finish_request = false;
    it->finish_header = false; // reset the client
    it->request->response_ready = false;
    it->requestHeader = "";
    it->requestBody = "";
    it->request->BodyLength = 0;
    it->request->HeaderState = 0;
    it->request->Path = "";
    it->request->Version = "";
    it->request->Request_header.clear();
    it->request->Uri = "";
    it->request->Response.clear();
    it->request->htype = 0;
    it->request->boundary = "";
    it->response.clear();
    // reset chnked element of post
    it->request->ChunkHexStr = "";
    it->request->HexaChunkStatus = 0;
    it->request->ChunkSize = 0;
    it->request->PostFd = 0;
    it->request->AmountRecv = 0;
    it->request->PostPath = "";
    it->request->MimeType = "";
    it->BodySize = 0;
    it->request->requested_file_fd = -1;
    it->request->is_header_sent = false;
    if (it->file_body.is_open())
        it->file_body.close();
}
int Server::Isset(fd_set &fd_read, fd_set &fd_write, fd_set &fd_tread, fd_set &fd_twrite, int maxfds)
{
    maxfd = maxfds;
    for (std::vector<Client>::iterator it = Clients.begin(); it != Clients.end();)
    {
        int client_fd = it->clien_socket;
        if (FD_ISSET(client_fd, &fd_tread))
        {
            if (this->ReceivesRequest(it))
            {
                ClearClient(fd_read, fd_write, it);
                continue;
            }
            if (it->finish_request == 1)
                Swapfd(client_fd, fd_write, fd_read);
        }
        else if (FD_ISSET(client_fd, &fd_twrite) && it->request->response_ready == true)
        {
            if (this->HandleResponse(it))
            {
                ClearClient(fd_read, fd_write, it);
                continue;
            }
            if (it->finish_response)
            {
                if (it->request->Request_header["Connection"] == "close")
                {
                    ClearClient(fd_read, fd_write, it);
                    continue;
                }
                Swapfd(client_fd, fd_read, fd_write);
                Reset(it);
            }
        }
        else if (time(NULL) - it->cliantime > 120)
        { // Handle timeout
            ClearClient(fd_read, fd_write, it);
            continue;
        }
        it++;
    }
    return maxfd; 
}

void Server::CheckBodyFinish(std::string message, std::vector<Client>::iterator it)
{
    if (it->request->htype == 1) // Chunked data
    {
        std::cout << "Chunked data\n"
                  << std::endl;
        it->requestBody.append(message);                // append the message to the body
        size_t pos = it->requestBody.find("0\r\n\r\n"); // check if the body contains the end of the chunked data
        if (pos != std::string::npos)                   // if it does, then the request is finished
        {
            size_t nextLinePos = it->requestBody.find("\r\n", pos + 3);     // check if there is any other data after the end of the chunked data
            if (nextLinePos != std::string::npos && nextLinePos == pos + 3) // if there is no other data, then the request is finished
                it->finish_request = true;                                  // set the finish_request flag to true
        }
    }
    else
    { // Non-chunked data
        it->requestBody.append(message);
        if (it->request->htype == 2 && it->requestBody.size() >= it->request->BodyLength)
        { // form data check if the body is finished
            it->finish_request = true;
        }
        else if (it->request->htype == 3 && it->requestBody.find(it->request->boundary) != std::string::npos) // multipart form data check if the body is finished
        {
            std::cout << "boundary found\n"
                      << std::endl;
            it->finish_request = true;
        }
    }
}

// fill the body, and check if the request is finished
void Server::checkBody(std::vector<Client>::iterator it)
{
    it->BodySize += it->requestBody.size();
    it->file_body << it->requestBody;
    if (it->file_body.fail())
    {
        std::cerr << "Error: failed to write to file " << std::endl;
        // it->request->Response = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
        it->request->sendCodeResponse("500");
        it->response_ready = true;
        return;
    }
    if (it->request->htype == ContentLength && it->BodySize == it->request->BodyLength)
    {
        it->finish_request = true;
    }
    else
    {
        size_t endOfBody = it->requestBody.find("\r\n0\r\n\r\n");
        if (endOfBody != std::string::npos && endOfBody + 7 == it->requestBody.size())
        {
            std::cout << "BODY SIZE " << it->BodySize << std::endl;
            it->finish_request = true;
        }
    }
}

void Server::CheckRequest2(std::string message, std::vector<Client>::iterator it)
{
    if (!it->finish_header)
    {
        it->finish_header = true;
        size_t endOfHeaders = message.find("\r\n\r\n");
        it->requestBody = message.substr(endOfHeaders + 4);
        it->requestHeader = message.substr(0, endOfHeaders);
        it->finish_request = it->request->parseRequestHeader(it->requestHeader);
        if (it->finish_request)
            return;

        it->create_file(it->request->Request_header);
    }
    else
        it->requestBody = message;

    checkBody(it);
}

int Server::handleRedirections(std::vector<Client>::iterator it)
{
    if (it->request->_Location_Scoop.redir.first != -1)
    {
        std::string code = to_string(it->request->_Location_Scoop.redir.first);
        std::string url = it->request->_Location_Scoop.redir.second;
        if (!it->request->_Location_Scoop.redir.second.empty())
        {
            it->request->Response = "HTTP/1.1 " + code + " " + it->request->_linker.Status_codes_error[code] + "\r\n";
            it->request->Response.append("Location: ");
            it->request->Response.append(url + "\r\n");
            it->request->Response.append("\r\n");
            it->finish_response = true;
        }
        else
            it->request->sendCodeResponse(code);
        return 1;
    }
    return 0;
}

int Server::ReceivesRequest(std::vector<Client>::iterator it)
{
    int clientfd = it->clien_socket;
    std::vector<char> buffer(BUFSIZE);
    // std::cout << "Start 1\n";
    int bytesRead = recv(clientfd, buffer.data(), buffer.size(), 0);
    if(HandleSendRecvResult(Recv,bytesRead,clientfd))
        return 1;
    it->cliantime = time(NULL);
    std::string header(buffer.begin(), buffer.begin() + bytesRead);
    header.resize(bytesRead);
    // it->request->setData();
    CheckRequest2(header, it);

    if (it->finish_request && !it->response_ready)
    {
        if (handleRedirections(it))
            return 0;
        if (it->request->Method == "GET")
            return it->request->GET();
        else if (it->request->Method == "POST")
            return it->request->POST(it.base());
        else if (it->request->Method == "DELETE")
        {
            it->request->DELETE(it->request->Path.c_str());
            return 0;
        }
    }
    return 0;
}

int Server::HandleResponse(std::vector<Client>::iterator it)
{
    int bytesSent;
    if (!it->request->is_header_sent)
    {
        bytesSent = send(it->clien_socket, it->request->Response.c_str(), it->request->Response.size(), 0);
        if(HandleSendRecvResult(Send,bytesSent,it->clien_socket))
            return 1;
        it->request->is_header_sent = true;
        if (it->request->requested_file_fd == -1) // requested_file_fd is -1 when there is no body to send
            it->finish_response = true;   
        return 0;
    }
    char buff[BUFSIZE];
    // std::cout << "Start sending body " << it->request->requested_file_fd << "\n";
    int bytesread = read(it->request->requested_file_fd, buff, BUFSIZE);
    if (bytesread < 0)
    {
        std::cerr << "Failed to read from file -> " << it->request->requested_file_fd << std::endl;
        return 1;
    }
    else if (bytesread == 0)
    {
        std::cout << "close file descriptor " << it->request->requested_file_fd << std::endl;
        close(it->request->requested_file_fd);
        it->request->requested_file_fd = -1;
        it->request->is_header_sent = false;
        it->finish_response = true;
        return 0;
    }
    bytesSent = send(it->clien_socket, buff, bytesread, 0);
    if(HandleSendRecvResult(Send,bytesSent,it->clien_socket))
        return 1;
    return (0);
}

void Server::CheckTime(fd_set &fd_read, fd_set &fd_write)
{
    for (std::vector<Client>::iterator it = Clients.begin(); it != Clients.end();)
    {
        if (time(NULL) - it->cliantime > 120)
        {
            ClearClient(fd_read, fd_write, it);
            continue;
        }
        it++;
    }
}
Server::~Server()
{
    close(server_socket);
}
