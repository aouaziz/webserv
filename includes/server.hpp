#ifndef SERVER_HPP
# define SERVER_HPP

#include "client.hpp"
#include "include.hpp"

class server{
    private :
    client tmp;
    std::map<int,client> ClientMap;
    std::string port;
    std::string ip;
    int server_socket;//SERVER FILE DESCRIPTOR
    struct addrinfo *servinfo;
    struct timeval time;
    fd_set fd_r,fd_w,fd_tr,fd_tw;
    int maxfd;
    public :
    server();
    void ServInit();
    void SelectInit();
    void check(int namber ,std::string error);
    int CreatServer();
    void AcceptClient(int c);
    void HandleRequest(int c);
    void HandleResponse(int c);
    ~server();

};

#endif