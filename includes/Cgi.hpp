#ifndef CGI_HPP
#define CGI_HPP
#include "Config.hpp"

class Cgi {
    private:
        LocationConfig 	const &_config;
        std::map<std::string, std::string> &_header;
        std::string    check_ext(std::string ext);
        std::vector<std::string> get_env(const std::string &file_path);
        // int fd; // <= we store the body of the response in this file descriptor

    public:
      Cgi(LocationConfig const &config, std::map<std::string, std::string>  &header);
      ~Cgi();
    //   Cgi(const Cgi & );
      std::string header;
      std::string body;
      std::string buff;
      int status_code;

      int   execute_cgi(std::string file_path, std::string ext);

};


#endif