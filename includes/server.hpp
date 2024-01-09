#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <sys/socket.h>
# include <sys/types.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <string.h>
# include <vector>
# include <sys/stat.h>
# include <sstream>
# include <fstream>
#define SOCKETERROR -1
#define BUFSIZE 1024

class server{
    private :
    int port;
    std::string ip;
    int server_socket;//SERVER FILE DESCRIPTOR
    struct sockaddr_in address;
    public :
    server();
    void ServInit();
    void check(int namber ,std::string error);
    int CreatServer(int port,char * ip);
    ~server();

};

#endif