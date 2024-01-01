#include "webserv.hpp"


webserv::webserv()
{
    
}

int webserv::getServFd()
{
    return (server_fd);
}

void webserv::setServFd(int fd)
{
    server_fd = fd;
}

webserv::~webserv()
{
    close(server_fd);
}

void webserv::start()
{
    std::string hello = "Hello from serer";
    int addrlen = sizeof(address);
    if((server_fd =  socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
       throw std::runtime_error("Socket creation failed");
    }
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);//port
    address.sin_addr.s_addr = INADDR_ANY;//ip address
    if(bind(server_fd, (struct sockaddr *)&address, addrlen) < 0)
    {
        throw std::runtime_error("bind failed");
    }
    if(listen(server_fd, 3) < 0)
    {
        throw std::runtime_error("listen failed");
    }
    while (true)
    {
        std::cout <<"\n+++++++ Waiting for new connection ++++++++\n\n";

        if((client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
        {
            throw std::runtime_error("accept failed");
        }
        char baffer[30000] = {0};
        int r = recv(client_fd, baffer, 30000,0);
        if(r < 0)
        {
            throw std::runtime_error("read failed");
        }
        std::cout << baffer << std::endl;
        int s = send(client_fd, hello.c_str(), hello.size(), 0);
        if(s < 0)
        {
            throw std::runtime_error("send failed");
        }
        std::cout << "Message sent\n" << std::endl;
        close(client_fd);
    }
}