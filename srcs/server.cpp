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
    time.tv_sec = 0;
    time.tv_usec = 0;
    // telling select to Return immediately after checking the descriptors
    FD_ZERO(&fd_r);
    FD_ZERO(&fd_w);
    FD_SET(server_socket,&fd_r);
    FD_SET(server_socket,&fd_w);
    maxfd = server_socket;
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
    check(fcntl(server_socket,F_SETFL,O_NONBLOCK),"fcntl failed");//setting the server in a non-blocking state
    int opt = 1;
    check(setsockopt(server_socket,SOL_SOCKET,SO_REUSEPORT,&opt,sizeof(opt)),"SETSOCKOPT failed ");//fix bind error
    check(bind(server_socket,(struct sockaddr *)&address,sizeof(address)),"bind failed");
    check(listen(server_socket,FD_SETSIZE),"listen failed");
    std::cout << "Server listening on port " << port << std::endl;
    while (true)
    {
        fd_tr = fd_r;
        fd_tw = fd_w;
        int client_socket = select(maxfd + 1,&fd_tr,&fd_tw,NULL,&time);
        if(client_socket == -1)
            std::cerr<<"Select error\n";
        
    }
    
}