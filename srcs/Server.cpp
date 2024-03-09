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
        HandleSocketError(-1, "getaddrinfo error");
}

void Server::CreatServer()
{
    ServInit();
    HandleSocketError((server_socket = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)), "Socket creation failed");
    HandleSocketError(fcntl(server_socket, F_SETFL, O_NONBLOCK), "fcntl failed"); // setting the server in a non-blocking state
    int opt = 1;
    HandleSocketError(setsockopt(server_socket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)), "SETSOCKOPT failed "); // fix bind error
    HandleSocketError(bind(server_socket, servinfo->ai_addr, servinfo->ai_addrlen), "bind failed");
    HandleSocketError(listen(server_socket, SOMAXCONN), "listen failed");
    freeaddrinfo(servinfo);
}

int Server::AcceptClient(fd_set &fd_read)
{
    if (Clients.size() >= 1022)
    {
        std::cerr << "Error: Maximum number of clients reached. Cannot accept new connections." << std::endl;
        return -1;
    }

    Client tmp(serverConfig);
    tmp.clien_socket = accept(server_socket, (struct sockaddr *)&tmp.new_addr, (socklen_t *)&tmp.addrlen);
    if (tmp.clien_socket == -1)
    {
        std::cerr << "unable to establish a connection to the clein on port" << this->port << std::endl;
        return -1;
    }
    HandleSocketError(fcntl(tmp.clien_socket, F_SETFL, O_NONBLOCK), "fcntl failed"); // setting the server in a non-blocking state
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
    maxfds--;
}

void Server::Swapfd(int fd, fd_set &se, fd_set &cl)
{
    FD_SET(fd, &se);
    FD_CLR(fd, &cl);
}

int Server::Isset(fd_set &fd_read, fd_set &fd_write, fd_set &fd_tread, fd_set &fd_twrite,int maxfd)
{
    maxfds = maxfd;
    for (std::vector<Client>::iterator it = Clients.begin(); it != Clients.end();)
    {
        int client_fd = it->clien_socket;
        if (FD_ISSET(client_fd, &fd_tread))
        {
            if (it->ReceivesRequest())
            {
                ClearClient(fd_read, fd_write, it);
                continue;
            }
            if (it->finish_request == true)
            {
                Swapfd(client_fd, fd_write, fd_read);
            }
        }
        else if (FD_ISSET(client_fd, &fd_twrite) && it->request->response_ready == true)
        {
            if (it->HandleResponse())
            {
                ClearClient(fd_read, fd_write, it);
                continue;
            }
            if (it->finish_response)
            {
                Swapfd(client_fd, fd_read, fd_write);
                it->ResetClient();
            }
        }
        else if (time(NULL) - it->cliantime > 120)
        { // Handle timeout
            ClearClient(fd_read, fd_write, it);
            continue;
        }
        it++;
    }
    return maxfds;
}

int Server::CheckTime(fd_set &fd_read, fd_set &fd_write,int maxfd)
{
    maxfds = maxfd;
    for (std::vector<Client>::iterator it = Clients.begin(); it != Clients.end();)
    {
        if (time(NULL) - it->cliantime > 120)
        {
            ClearClient(fd_read, fd_write, it);
            continue;
        }
        it++;
    }
    return maxfds;
}
Server::~Server()
{
    close(server_socket);
}
