#include "../includes/Server.hpp"

Server::Server(const ServerConfig &serverConfig) : serverConfig(serverConfig)
{
    Clients.reserve(10);
}

bool Server::handelSendAndRecv(int type, int namber)
{
    if (namber == 0)
    {
        std::cerr << "Connection closed by peer." << std::endl;
        return true;
    }
    else if (namber == -1)
    {
        std::cerr << "server can't " << ((type == Send) ? "send to " : "recv from ") << "client \n";
        return true;
    }
    return false;
}

void Server::ServInit()
{
    struct addrinfo address;
    port = serverConfig.port;
    ip = serverConfig.host;
    memset(&address, 0, sizeof(address)); // make sure the struct is empty
    address.ai_family = AF_INET;          // IPV4
    address.ai_socktype = SOCK_STREAM;    // TCP stream sockets
    address.ai_flags = INADDR_ANY;
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

int Server::AcceptClient(fd_set &fd_read)
{
    if (Clients.size() == 1023)
    {
        std::cerr << "Error: Maximum number of clients (1024) reached. Cannot accept new connections." << std::endl;
        return -1;
    }
    struct sockaddr_in new_addr;
    int addrlen = sizeof(struct sockaddr_in);
    Client tmp(serverConfig);
    tmp.client_socket = accept(server_socket, (struct sockaddr *)&new_addr, (socklen_t *)&addrlen);
    if (tmp.client_socket == -1)
    {
        std::cerr << "unable to establish a connection to the clein on port" << this->port << std::endl;
        return -1;
    }
    check(fcntl(tmp.client_socket, F_SETFL, O_NONBLOCK), "fcntl failed"); // setting the client in a non-blocking state
    tmp.clientime = time(NULL);                                           // time out for the client
    FD_SET(tmp.client_socket, &fd_read);
    Clients.push_back(tmp);
    return (tmp.client_socket);
}

void Server::ClearClient(fd_set &fd_read, fd_set &fd_write, std::vector<Client>::iterator &it)
{
    FD_CLR(it->client_socket, &fd_read);
    FD_CLR(it->client_socket, &fd_write);
    close(it->client_socket);
    if (it->file_body)
    {
        it->file_body->close();
        delete it->file_body;
    }
    if (it->chunk_output)
    {
        it->chunk_output->close();
        delete it->chunk_output;
    }
    it = Clients.erase(it);
}
void Server::Swapfd(int fd, fd_set &se, fd_set &cl)
{
    FD_SET(fd, &se);
    FD_CLR(fd, &cl);
}
void Server::Reset(std::vector<Client>::iterator it)
{
    it->finish_response = false;
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
    it->request->MimeType = "";
    it->BodySize = 0;
    it->request->requested_file_fd = -1;
    it->request->is_header_sent = false;
    if (it->file_body && it->file_body->is_open())
        it->file_body->close();

    it->file_name = "";

    it->cgi_pid = -1;
    it->is_cgi_running = false;

    it->has_chunked_body = false;
    if (it->chunk_output && it->chunk_output->is_open())
        it->chunk_output->close();
}

void check_on_cgi(Client &client)
{
    const int TIMEOUT = 5;
    int status;
    pid_t res = waitpid(client.cgi_pid, &status, WNOHANG); // WNOHANG makes waitpid non blocking

    if (res == 0) // process is still running
    {
        struct timeval current_time;
        gettimeofday(&current_time, NULL);
        struct timeval expiry_time;
        expiry_time.tv_sec = client.cgi_start_time.tv_sec + TIMEOUT;
        expiry_time.tv_usec = client.cgi_start_time.tv_usec;

        if (expiry_time.tv_sec > current_time.tv_sec ||
            (expiry_time.tv_sec == current_time.tv_sec && expiry_time.tv_usec > current_time.tv_usec))
        {
            // timeout not yet
            // move on do nothing
            return;
        }
        else
        {
            // timeout elapsed
            kill(client.cgi_pid, SIGKILL);
            waitpid(client.cgi_pid, NULL, 0);
            client.cgi_pid = -1;
            client.is_cgi_running = false;
            std::cerr << "timeout elapsed " << std::endl;
            client.request->sendCodeResponse("504");
            std::string cgi_file_name = "/tmp/out_" + to_string(client.client_socket);
            unlink(cgi_file_name.c_str());
            if (client.request->Method == "POST")
                unlink(client.file_name.c_str());
            return;
        }
    }
    else if (res == client.cgi_pid)
    {
        // finished execution
        if (WIFEXITED(status))
        {

            // Child process terminated normally
            int exit_status = WEXITSTATUS(status);
            if (exit_status == 0)
            {
                client.request->Response = Cgi::finish_cgi_response(client);
                client.cgi_pid = -1;
                client.is_cgi_running = false;
                std::string cgi_file_name = "/tmp/out_" + to_string(client.client_socket);
                unlink(cgi_file_name.c_str());
                if (client.request->Method == "POST")
                    unlink(client.file_name.c_str());
                return;
            }
        }
    }
    else
    {
        std::cerr << "killing the cgi_pid" << std::endl;
        kill(client.cgi_pid, SIGKILL);
        waitpid(client.cgi_pid, NULL, 0);
    }
    client.cgi_pid = -1;
    client.is_cgi_running = false;
    std::cerr << "waitpid error " << std::endl;
    client.request->sendCodeResponse("502");
    std::string cgi_file_name = "/tmp/out_" + to_string(client.client_socket);
    unlink(cgi_file_name.c_str());
    if (client.request->Method == "POST")
        unlink(client.file_name.c_str());
}

void handle_chunked_finished(Client &client)
{
    client.file_body->close(); // we close the old file and deleted, as we dont need it anymore
    client.chunk_output->close();
    unlink(client.file_name.c_str());
    client.file_name = client.chunk_file_name;

    client.request->POST(&client);
    client.has_chunked_body = false;
    delete client.chunk_output;
    client.chunk_output = NULL;
}

void handle_chunked(Client &client)
{
    char buffer[BUFSIZE];

    if (!client.file_body->eof())
    {
        client.file_body->read(buffer, BUFSIZE);                      // check later
        client.chunk_data.append(buffer, client.file_body->gcount()); // only include information that we have read
        int res = client.request->process_buffered_data(client.chunk_data, client.chunk_size, *client.chunk_output);
        if (res == 0)
            handle_chunked_finished(client);
    }
    else
    {
        handle_chunked_finished(client);
    }
}

void Server::Isset(fd_set &fd_read, fd_set &fd_write, fd_set &fd_tread, fd_set &fd_twrite)
{
    for (std::vector<Client>::iterator it = Clients.begin(); it != Clients.end();)
    {
        int client_fd = it->client_socket;
        if (FD_ISSET(client_fd, &fd_tread))
        {
            if (this->ReceivesRequest(it))
            {
                ClearClient(fd_read, fd_write, it);
                continue;
            }
            if (it->finish_request == 1 || it->request->Request_header["stop_receiving"] == "true")
            {
                Swapfd(client_fd, fd_write, fd_read);
            }
        }
        else if (FD_ISSET(client_fd, &fd_twrite) && it->request->response_ready == true)
        {
            if (it->is_cgi_running)
            {
                check_on_cgi(*it);
            }
            else if (it->has_chunked_body)
            {
                handle_chunked(*it);
            }
            else if (this->HandleResponse(it) || it->request->Request_header["stop_receiving"] == "true") // drop client after header is finished
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
        else if (time(NULL) - it->clientime > 120)
        { // Handle timeout
            ClearClient(fd_read, fd_write, it);
            continue;
        }
        it++;
    }
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
            it->request->response_ready = true;
        }
        else
            it->request->sendCodeResponse(code);
        return 1;
    }
    return 0;
}

// fill the body, and check if the request is finished
void Server::checkBody(std::vector<Client>::iterator it)
{
    it->BodySize += it->requestBody.size();
    if (it->BodySize > to_number(it->request->_Location_Scoop.client_max_body_size.c_str())) // Entity too large
    {
        it->request->sendCodeResponse("413");
        it->file_body->close();
        unlink(it->file_name.c_str());
        it->finish_request = true;
        it->request->response_ready = true;
        it->request->Request_header["stop_receiving"] = "true";

        return;
    }
    *it->file_body << it->requestBody;
    if (it->file_body->fail())
    {
        std::cerr << "Error: failed to write to file " << std::endl;
        it->request->sendCodeResponse("500");
        it->request->response_ready = true;
        it->request->Request_header["stop_receiving"] = "true";
        it->finish_request = true;
        return;
    }
    if (it->request->htype == ContentLength && it->BodySize == it->request->BodyLength)
    {
        it->finish_request = true;
    }
    else // chunked data
    {
        size_t endOfBody = it->requestBody.find("\r\n0\r\n\r\n");
        if (endOfBody != std::string::npos && endOfBody + 7 == it->requestBody.size())
        {
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
        if (endOfHeaders == std::string::npos)
        {
            it->finish_request = true;
            it->request->Request_header["stop_receiving"] = true;
            return;
        }
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

int Server::ReceivesRequest(std::vector<Client>::iterator it)
{
    int clientfd = it->client_socket;
    std::vector<char> buffer(BUFSIZE);
    ssize_t bytesRead = recv(clientfd, buffer.data(), buffer.size(), 0);
    if (handelSendAndRecv(Recv, bytesRead))
        return -1;
    it->clientime = time(NULL);                   // re set client time
    std::string header(buffer.data(), bytesRead); // Correct construction of string
    CheckRequest2(header, it);

    if (it->finish_request && !it->request->response_ready)
    {

        if (handleRedirections(it))
            return 0;
        if (it->request->Method == "GET")
            return it->request->GET(*it);
        else if (it->request->Method == "POST")
            return it->request->POST(&(*it));
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
        bytesSent = send(it->client_socket, it->request->Response.c_str(), it->request->Response.size(), 0);
        if (handelSendAndRecv(Send, bytesSent))
            return -1;
        it->request->is_header_sent = true;
        if (it->request->requested_file_fd == -1) // requested_file_fd is -1 when there is no body to send
            it->finish_response = true;
        return 0;
    }
    char buff[BUFSIZE];
    int bytesread = read(it->request->requested_file_fd, buff, BUFSIZE);
    if (bytesread < 0)
    {
        std::cerr << "Failed to read from file  " << std::endl;
        return 1;
    }
    else if (bytesread == 0)
    {
        close(it->request->requested_file_fd);
        it->request->requested_file_fd = -1;
        it->request->is_header_sent = false;
        it->finish_response = true;
        return 0;
    }
    bytesSent = send(it->client_socket, buff, bytesread, 0);
    if (handelSendAndRecv(Send, bytesSent))
        return -1;
    return (0);
}

void Server::CheckTime(fd_set &fd_read, fd_set &fd_write)
{
    for (std::vector<Client>::iterator it = Clients.begin(); it != Clients.end();)
    {
        if (time(NULL) - it->clientime > 120)
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
