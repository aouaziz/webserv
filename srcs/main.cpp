
#include "../includes/webserv.hpp"


int main(int argc, char **argv)
{
    if (argc != 2)
    {
        (void)argv;
        std::cout << "Usage: ./webserv [config_file]" << std::endl;
        return (1);
    }
    webserv server;
    server.Start(); 
}
