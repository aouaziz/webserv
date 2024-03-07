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
        //config.printConfig();
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

// int main() {
    
//     // 1. Create a server config
//     ServerConfig serverConfig;
//     // 2. Set the server config
//     HTTPMethod httpMethod(serverConfig);
//     // 3. Open a file to write to
//     int fd = open("post.txt", O_WRONLY | O_CREAT, 0644);
//     if (fd == -1) {
//         std::cerr << "Failed to open file\n";
//         return 1;
//     }
//     // 4. Set the file descriptor
//     httpMethod.PostFd = fd;
//     // 5. Create a body to send
//     const char* body = "4\r\nWiki\r\n5\r\npedia\r\nE\r\n in\r\n\r\nchunks.\r\n0\r\n\r\n";
//     // 6. Get the length of the body
//     int bytesreceived = strlen(body);
//     // 7. Call the chunkes method
//     int result = httpMethod.chunkes(body, bytesreceived);
//     // 8. Print the result
//     std::cout << "Result: " << result << std::endl;
//     // 9. Print the contents of the file
//     std::ifstream postFile("post.txt");
//     // 10. Print the contents of the file
//     std::string line;
//     // 11. Print the contents of the file
//     while (getline(postFile, line)) {
//         std::cout << line << std::endl;
//     }
//     // 12. Close the file descriptor
//     close(fd);
//     return 0;
// }