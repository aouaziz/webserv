#include "../includes/Webserv.hpp"


int main(int ac, char **av)
{
    if (ac != 2)
    {
        std::cout << "Usage: ./webserv <config_file>" << std::endl;
        return (1);
    }
    Config config(av[1]);
    try
    {
        config.parseConfig();
        config.check_config();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        if (!config.get_key().empty())
            std::cerr << "Error in key: " << config.get_key() << std::endl;
        return (1);
    }
    Webserv webserv(config);

    return (0);
}
