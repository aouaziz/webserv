#include "../includes/Cgi.hpp"

Cgi::Cgi(LocationConfig const &config, std::map<std::string, std::string> &header) : _config(config), _header(header)
{
}

Cgi::~Cgi()
{
}

void separateHeaderFromBody(const std::string &httpResponse, std::string &header, std::string &body)
{
    size_t pos = httpResponse.find("\r\n\r\n");
    if (pos != std::string::npos)
    {
        header = httpResponse.substr(0, pos);
        body = httpResponse.substr(pos + 4); // Skip the double newline characters
    }
    else
    {
        header = httpResponse;
        body = "";
    }
}

std::vector<std::string> split_env(std::string env)
{
    size_t pos = 0;
    std::vector<std::string> ret;
    while ((pos = env.find(":")) != std::string::npos)
    {
        ret.push_back(env.substr(0, pos));
        env.erase(0, pos + 1); // erase the first part of the string
    }
    ret.push_back(env); // push the last part of the string
    return (ret);
}

// matching the extention with the cgi path in the config file
std::string Cgi::check_ext(std::string ext)
{
    std::map<std::string, std::string>::const_iterator it = this->_config.cgi_ext.begin();

    while (it != this->_config.cgi_ext.end())
    {
        if (ext == it->first)
            return it->second;
        ++it;
    }
    return "";
}

std::vector<std::string> Cgi::get_env(const std::string &file_path)
{
    std::string env = "SCRIPT_FILENAME=:QUERY_STRING=:REQUEST_METHOD=:CONTENT_TYPE=:CONTENT_LENGTH=:REDIRECT_STATUS=:HTTP_COOKIE=";

    std::vector<std::string> env_vec = split_env(env);

    std::string content_type = "";
    std::string content_length = "0";

    if (this->_header["method"] == "POST")
    {
        content_type = this->_header["Content-Type"];
        content_length = this->_header["Content-Length"];
    }

    env_vec[0] += "./" + file_path;
    env_vec[1] += this->_header["query"];
    env_vec[2] += this->_header["method"];
    env_vec[3] += content_type;
    env_vec[4] += content_length;
    env_vec[5] += to_string(200);
    env_vec[6] += this->_header["Cookie"];

    return env_vec;
}

int Cgi::execute_cgi(Client &client, std::string file_path, std::string ext)
{
    std::string cgi_path = check_ext(ext.substr(1));
    if (cgi_path.empty())
    {
        if (this->_header["method"] == "POST")
            return 400;
        return -1;
    }

    // we check if the cgi file exists and executable
    if (access(cgi_path.c_str(), F_OK | X_OK) == -1)
    {
        return 404;
    }

    std::vector<std::string> vec_str = get_env(file_path);
    char *envp[8];

    for (size_t i = 0; i < vec_str.size(); i++)
    {
        envp[i] = (char *)vec_str[i].c_str();
    }
    envp[7] = NULL;

    char *argv[3] = {(char *)cgi_path.c_str(), (char *)file_path.c_str(), NULL};

    std::string buff;
    std::string file_name = this->_header["body_file"];

    int size = to_number(this->_header["Content-Length"].c_str());
    (void)size;

    std::string cgi_file_name = "/tmp/out_" + to_string(client.client_socket);
    int out = open(cgi_file_name.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    int pid = fork();
    if (pid < 0)
    {
        std::cerr << "cgi fail()\n";
        return 500;
    }
    // Child process
    if (pid == 0)
    {

        if (this->_header["method"] == "POST")
        {
            int fd_in = open(file_name.c_str(), O_RDONLY);

            if (dup2(fd_in, STDIN_FILENO) < 0)
                std::cerr << "dup2 failed" << std::endl;
            close(fd_in);
        }

        dup2(out, 1);
        dup2(out, 2);
        close(out);

        // Execute the CGI script
        int status_code = execve(argv[0], argv, envp);
        // If execve fails, print error message and exit
         std::cerr << "cgi fail()\n";
        exit(status_code);
    }
    close(out);
    client.is_cgi_running = true;
    client.cgi_pid = pid;
    gettimeofday(&client.cgi_start_time, NULL);
    return 0;
}

std::string Cgi::finish_cgi_response(Client &client)
{
    // Read from the pipe and store the response in 'buff'
    char chunk[4096];
    std::string buff, header, body;
    ssize_t bytes_read;
    // this is blocking, should be handled by select
    std::string cgi_file_name = "/tmp/out_" + to_string(client.client_socket);

    int out = open(cgi_file_name.c_str(), O_RDONLY);
    while ((bytes_read = read(out, chunk, sizeof(chunk))) > 0)
        buff.append(chunk, bytes_read);
    // Close file descriptors
    close(out);
    unlink(cgi_file_name.c_str());

    separateHeaderFromBody(buff, header, body);
    std::string httpHeader = "HTTP/1.1 200 OK\r\n";
    header.insert(0, "Content-Length: " + to_string(body.size()) + "\r\n");
    header.insert(0, httpHeader);
    header.append("\r\n\r\n");
    header.append(body);
    client.request->response_ready = true;
    return header;
}

