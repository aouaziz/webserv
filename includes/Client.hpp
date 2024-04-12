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
    std::fstream file_body;
    std::vector<std::string> response;
    struct sockaddr_in new_addr;
    time_t cliantime;

    bool finish_response;
    bool  response_ready;
    bool finish_request;
    bool finish_header;
    bool ConnectionClose;
    bool to_erase;//need

    size_t BodySize;
    size_t pos;
    size_t size_of_chunk;

    int addrlen ;
    int clien_socket;

    std::string requestHeader;
    std::string requestBody;
    std::string file_name;
    std::string chunk_remaining;
    std::string chunk_data;

    Client(ServerConfig serverConfig);
    Client(Client const &src);
    Client &operator=(Client const &src);
    ~Client(); 

    // recv request
    int     ReceivesRequest();
    int     handleRedirections();
    void    ProcessAndValidateBody();
    bool    HandleClientMaxBodySize();
    void    HandleRequestBody(std::string message);
    void    HandleContentLengthPost();
    void    HandleChunkedPost();
    bool    eraseProcessedData();
    bool    processRemainingData();
    void    processChunkedData();
    
    // send response
    int     HandleResponse();
    void    ResetClient();
    void	GenerateRequestFile(MapOf_Str_Str	&Request_header);
    void    delete_file();

    bool handelSendAndRecv(int type, int namber);
};
#endif