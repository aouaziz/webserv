#ifndef SERVER_HPP
# define SERVER_HPP


#include "include.hpp"
#include "Config.hpp"
#include "Client.hpp"
class Client;
enum SendRecv
{
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
        int         flag;
        int maxfd;
        Server(const ServerConfig &serverConfig);

        void CheckRequest2(std::string message,std::vector<Client>::iterator it);
        void CheckRequest(std::string message,std::vector<Client>::iterator it);
        void CheckBodyFinish(std::string message,std::vector<Client>::iterator it);
        void ServInit();
        void CheckRequest(long long bytesRead,std::string message,std::vector<Client>::iterator it);
        void CreatServer();
        int AcceptClient(fd_set &fd_read);
        int ReceivesRequest(std::vector<Client>::iterator it);
        void CheckTime(fd_set &fd_read, fd_set &fd_write);
        int HandleResponse(std::vector<Client>::iterator it );
        int Isset(fd_set &fd_read,fd_set &fd_write,fd_set &fd_tread,fd_set &fd_twrite, int maxfds);
        void ClearClient(fd_set &fd_read,fd_set &fd_write,std::vector<Client>::iterator &it);
        void Swapfd(int fd,fd_set &se,fd_set &cl);
        void Reset(std::vector<Client>::iterator it);
        void checkBody(std::vector<Client>::iterator it);
        int handleRedirections(std::vector<Client>::iterator it);
        bool HandleSendRecvResult(int type, int numBytes, int clientfd);
        ~Server();
};

#endif