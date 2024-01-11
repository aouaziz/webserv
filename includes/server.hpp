#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <fstream>
#include <client.hpp>
#include <map>
#include <vector>
#include <cstring>
#include <sstream>
#define SOCKETERROR -1
#define BUFSIZE 1024

class server{
    private :
    std::string port;

    std::string ip;
    int server_socket;//SERVER FILE DESCRIPTOR
    struct addrinfo *servinfo;
    struct timeval time;
    fd_set fd_r,fd_w,fd_tr,fd_tw;
    int maxfd;
    client tmp;
    std::map<int,client> ClientMap;
    public :
    server();
    void ServInit();
    void SelectInit();
    void check(int namber ,std::string error);
    int CreatServer(int port,char * ip);
    void AcceptClient(int c);
    void HandleRequest(int c);
    void HandleResponse(int c);
    ~server();

};

#endif