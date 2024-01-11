
#include "../includes/server.hpp"


int main(int argc, char **argv)
{
    if (argc != 2)
    {
        (void)argv;
        std::cout << "Usage: ./webserv [config_file]" << std::endl;
        return (1);
    }
    server test(80,"127.0.0.1");
    test.CreatServer();
}
