#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "include.hpp"

struct LocationConfig
{
    std::string path;
    std::string root;
    std::string index;
    bool autoindex;
    std::vector<std::string> methods;
    std::string client_max_body_size;
    std::map<std::string, std::string> error_pages;
    std::pair<int, std::string> redir;

    bool upload;
    std::string upload_path;

    bool cgi;
    std::map<std::string, std::string> cgi_ext;//first is extention second is path
};

struct ServerConfig
{
    std::string host;
    std::string port;
    std::string server_name;
    std::vector<LocationConfig> locations;
    LocationConfig common;//
};



class Config
{
    public:
    //private :
        std::string file_name;
        bool server_brackets;
        bool location_brackets;
        std::string last_key, key;
        std::vector<std::string> ports;
        ServerConfig current_server;
        LocationConfig current_location;
        std::vector<ServerConfig> servers;


        Config(std::string file_name);
        Config();
        
        Config &operator=(const Config& object);
        ~Config();

        const std::string & get_key() const;
        const std::vector<ServerConfig> & get_servers() const;

        void check_config();

        void handle_server(std::stringstream &ss);
        void handle_openBracket(std::stringstream &ss);
        void handle_closedBracket(std::stringstream &ss);
        void handle_listen(std::stringstream &ss);
        void handle_host(std::stringstream &ss);
        void handle_server_name(std::stringstream &ss);
        void handle_errorPages(std::stringstream &ss);
        void handle_maxBodySize(std::stringstream &ss);
        void handle_root(std::stringstream &ss);
        void handle_location(std::stringstream &ss);
        void handle_methods(std::stringstream &ss);
        void handle_autoindex(std::stringstream &ss);
        void handle_index(std::stringstream &ss);
        void handle_return(std::stringstream &ss);
        void handle_upload(std::stringstream &ss);
        void handle_upload_path(std::stringstream &ss);
        void handle_cgi(std::stringstream &ss);
        void handle_cgi_ext(std::stringstream &ss);

        void parseConfig();
        void printConfig();
};

#endif