
#ifndef WEBSERV_HPP
# define WEBSERV_HPP


#include "server.hpp"

#define PORT 4242
#define MAX_CLIENTS 10



class webserv {
    private:
    int server_socket;//SERVER FILE DESCRIPTOR
    struct sockaddr_in address;
    public:
    webserv();
    void Erorr(std::string error);
    void Start();
    ~webserv();
};

#endif