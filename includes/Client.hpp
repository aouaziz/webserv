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
    bool finish_request;
    bool finish_header;
    size_t BodySize;
    int client_socket;
    std::string requestHeader;
    std::string requestBody;
    std::fstream *file_body;
    std::string file_name;
    time_t clientime;

    bool    is_cgi_running;
    pid_t cgi_pid;
    struct timeval cgi_start_time;

    bool            has_chunked_body;
    std::string     chunk_data;
    size_t          chunk_size;
    std::string     chunk_file_name;
    std::ofstream   *chunk_output;


    Client(ServerConfig serverConfig);
    Client(Client const &src);
    Client &operator=(Client const &src);
    void	create_file(MapOf_Str_Str	&Request_header);
    void    delete_file();
    ~Client();
};
#endif