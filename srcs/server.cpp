#include "../includes/server.hpp"

server::server(){}

server::~server(){
    close(server_socket);
}

void server::ServInit()
{
    memset(&address,0,sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = inet_addr(ip.c_str());
}

void server::check(int namber ,std::string error)
{
    if(namber == SOCKETERROR)
    {
        std::cerr << error << std::endl;
        exit(EXIT_FAILURE); 
    }
}

int server::CreatServer(int port, char* ip)
{
    check((server_socket = socket(AF_INET,SOCK_STREAM,0)),"Socket creation failed");
    ServInit();
    check(bind(server_socket,(struct sockaddr *)&address,sizeof(address)),"bind failed");
    check(listen(server_socket,FD_SETSIZE),"listen failed");
    
}