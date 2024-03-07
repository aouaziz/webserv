#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "include.hpp"
#include "HTTPMethod.hpp"
#include "Server.hpp"
#include "Request.hpp"


class HTTPMethod;


struct ServerConfig;

class Client{
public:
    HTTPMethod *request;
    std::vector<std::string> response;
    //HTTPMethod method;
    bool finish_response;
    bool  response_ready;
    bool finish_request;
    bool finish_header;
    size_t BodySize;
    int clien_socket;
    std::string requestHeader;
    std::string requestBody;
    std::vector<std::string> chunkedData;
    std::fstream file_body;
    std::string file_name;
    time_t cliantime;
    Client(ServerConfig serverConfig);
    Client(Client const &src);
    Client &operator=(Client const &src);
    void	create_file(MapOf_Str_Str	&Request_header);
    void    delete_file();
    ~Client();
};
#endif