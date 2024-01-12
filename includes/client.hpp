#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "include.hpp"

struct Request
{
    std::string method;
    std::string target;
    int httptype;
};
class client{

public:
    struct addrinfo *clienfo;
    int clien_socket;
    std::string rtmp;
    struct Request request;
    client();
    ~client();
};

#endif