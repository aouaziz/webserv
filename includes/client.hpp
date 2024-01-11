#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "server.hpp"

struct Request
{
    std::string method;
    std::string target;
    int httptype;
};



class client
{

public:
    struct addrinfo *clienfo;
    int clien_socket;
    std::string rtmp;
    struct Request request;
    client();
    client(const client &other);
    client& operator=(const client& other) ;
    ~client();
};

#endif