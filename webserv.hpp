
#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <iostream>
#include <sys/select.h>

#define PORT 4242
#define MAX_CLIENTS 10
class webserv {
    private:
    int server_socket;//SERVER FILE DESCRIPTOR
    struct sockaddr_in address;
    public:
    webserv();
    int getServFd();
    void setServFd(int fd);
    ~webserv();
    void start();
};

#endif