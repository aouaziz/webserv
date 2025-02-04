#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "include.hpp"
#include "Config.hpp"

class Request
{
public:
    ServerConfig   config;
    std::string Method;
    std::string Req_Uri;
    int BodyLength;
    std::map<std::string,std::string> headers;



    Request(ServerConfig   config);
    bool parse_request(std::string request_header);
    ~Request();
};






#endif