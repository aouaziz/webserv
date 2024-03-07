#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "include.hpp"
#include "Server.hpp"
#include "Config.hpp"

class Webserv{
private:
    std::vector<Server> servers;   
    fd_set fd_r,fd_tr,fd_w,fd_tw;
    int maxfd;
    timeval time;
    int max ;
    int Slecetfd;
public:
    Webserv(Config config);
    void start_select_loop(void);
    ~Webserv();
    void InitSelect();
};




#endif