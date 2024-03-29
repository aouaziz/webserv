#include "../includes/Webserv.hpp"


Webserv::Webserv(Config config)
{
    const std::vector<ServerConfig> &config_servers = config.get_servers();
    for (size_t i = 0; i < config_servers.size(); i++)
    {
        Server server(config.servers[i]);
        servers.push_back(server);
    }
    for (size_t i = 0; i < servers.size(); i++)
    {
        servers[i].CreatServer();
        std::cout << "server " << servers[i].ip << " is running on port " << servers[i].port << std::endl;
    }

    InitSelect();
    start_select_loop();
}

std::string to_string(size_t num) {
  std::stringstream ss;
  ss << num;
  return ss.str();
}

size_t to_namber(const char *num) {
    size_t ret;
    std::stringstream ss;
    ss << num;
    ss >> ret;
    return ret;
}

Webserv::~Webserv()
{
    servers.clear();
}
void Webserv::InitSelect()
{
    time.tv_sec = 120;
    time.tv_usec= 0;
    FD_ZERO(&fd_r);
    FD_ZERO(&fd_w);
    maxfd = -1;
    for (size_t i = 0; i < servers.size(); i++)
    {
        FD_SET(servers[i].server_socket,&fd_r);
        if(servers[i].server_socket > maxfd)
            maxfd = servers[i].server_socket;
    }
}


void Webserv::start_select_loop()
{
    
    signal(SIGPIPE, SIG_IGN);
    while (true)
    {
        fd_tr = fd_r;
        fd_tw = fd_w;
        Slecetfd = select(maxfd+1, &fd_tr, &fd_tw, NULL, &time);
        HandleSocketError(Slecetfd, "select error" );
        if (Slecetfd == 0)
        {
            for (size_t i = 0; i < servers.size(); i++)
                maxfd = servers[i].CheckTime(fd_r,fd_w,maxfd);
            continue;
        }
        for (size_t i = 0; i < servers.size(); i++)
        {
            if (FD_ISSET(servers[i].server_socket,&fd_tr))
                max =servers[i].AcceptClient(fd_r);
            else
                maxfd = servers[i].Isset(fd_r,fd_w,fd_tr,fd_tw,maxfd);
            if(max > maxfd)
                maxfd = max;
        }
    }
}

void HandleSocketError(int namber, const std::string& errorMessage)
{
    if (namber == SOCKETERROR)
    {
        std::cerr << errorMessage << std::endl;
        exit(EXIT_FAILURE);
    }
}