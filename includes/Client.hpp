#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "include.hpp"
#include "HTTP.hpp"
#include "Server.hpp"


class HTTP;


struct ServerConfig;

class Client{
public:
    HTTP *request;
    std::vector<std::string> response;
    //HTTP method;
    bool finish_response;
    bool  response_ready;
    bool finish_request;
    bool finish_header;
    struct sockaddr_in new_addr;
    int addrlen ;
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
    ~Client(); 
    // recv request
    int     ReceivesRequest();
    int     handleRedirections();
    void    checkBody();
    void    CheckRequest2(std::string message);
    // send response
    int     HandleResponse();
    void    ResetClient();
    void	create_file(MapOf_Str_Str	&Request_header);
    void    delete_file();

    bool handelSendAndRecv(int type, int namber);
};
#endif