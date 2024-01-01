
#include "webserv.hpp"


int main(int argc, char **argv)
{
    if (argc != 2)
    {
        (void)argv;
        std::cout << "Usage: ./webserv [config_file]" << std::endl;
        return (1);
    }
    try
    {
        webserv server;
        server.start();
        
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}
