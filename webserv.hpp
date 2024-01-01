
#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <iostream>
#include <string>

#define PORT 8080

class webserv {
    private:
    int server_fd;//SERVER FILE DESCRIPTOR
    int client_fd;//CLIENT FILE DESCRIPTOR
    struct sockaddr_in address;
    public:
    webserv();
    int getServFd();
    void setServFd(int fd);
    ~webserv();
    void start();
};

#endif