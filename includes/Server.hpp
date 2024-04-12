#ifndef SERVER_HPP
# define SERVER_HPP


#include "include.hpp"
#include "Config.hpp"
#include "Client.hpp"
class Client;

enum types{
    Send = 1,
    Recv
};

class Server {

    public :
        const ServerConfig &serverConfig;

        std::vector<Client> Clients;
        std::string port;
        std::string ip;
        int server_socket;
        addrinfo *servinfo;
        int         maxfds;
        Server(const ServerConfig &serverConfig);

        void ServInit();
        void CreatServer();
        int AcceptClient(fd_set &fd_read);
        int CheckTime(fd_set &fd_read, fd_set &fd_write,int maxfd);
        int Isset(fd_set &fd_read,fd_set &fd_write,fd_set &fd_tread,fd_set &fd_twrite,int maxfd);
        void ClearClient(fd_set &fd_read,fd_set &fd_write,std::vector<Client>::iterator &it);
        void Swapfd(int fd,fd_set &se,fd_set &cl);
        ~Server();
};

#endif