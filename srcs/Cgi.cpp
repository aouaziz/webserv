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
        header = httpResponse.substr(0, pos + 4);
        body = httpResponse.substr(pos + 4); // Skip the double newline characters
    }
    else
    {
        // If double newline is not found, treat the whole response as header
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
        env.erase(0, pos + 1);
    }
    ret.push_back(env);
    return (ret);
}

// matching the extention with the cgi path in the config file
std::string Cgi::check_ext(std::string ext)
{
    // std::cout << ext << std::endl;
    std::map<std::string, std::string>::const_iterator it = this->_config.cgi_ext.begin();

    while (it != this->_config.cgi_ext.end())
    {
        // std::cout << " ext :  " << it->first << ", path " << it->second << std::endl;

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

int Cgi::execute_cgi(std::string file_path, std::string ext)
{
    std::string cgi_path = check_ext(ext.substr(1));
    // std::cout << "CGI START " << std::endl;

    if (cgi_path.empty())
    {
        if (this->_header["method"] == "POST")
            return 400;
        return -1;
    }

    // we check if the cgi file exists and executable
    if (access(cgi_path.c_str(), F_OK | X_OK) == -1)
    {
        std::cerr << "Error: file does not exist" << std::endl;
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

    int size = to_namber(this->_header["Content-Length"].c_str());
    (void)size;

    int out = open("/tmp/out", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    int pid = fork();
    if (pid < 0)
    {
        write(2, "cgi fail()\n", 12);
        return 500;
    }
    // Child process
    if (pid == 0)
    {

        if (this->_header["method"] == "POST")
        {
            int fd_in = open(file_name.c_str(), O_RDONLY);

            if (dup2(fd_in, STDIN_FILENO) < 0)
                std::cout << "dup2 failed" << std::endl;
            close(fd_in);
        }

        dup2(out, 1);
        dup2(out, 2);
        close(out);

        // Execute the CGI script
        execve(argv[0], argv, envp);
        // If execve fails, print error message and exit
        write(2, "cgi fail()\n", 12);
        exit(1);
    }

    // Parent process
    // Wait for the child process to finish
    int status;
    waitpid(pid, &status, 0); // Wait for the child process to finish
    // if (result == 0)
    // {
    //     sleep(5);
    //     result = waitpid(pid, &status, WNOHANG);
    //     if (result == 0)
    //     {
    //         // Child process did not finish within the timeout, terminate it
    //         kill(pid, SIGKILL);
    //         return 500;
    //     }
    // }
    if (WIFEXITED(status))
    {
        // Child process terminated normally
        int exit_status = WEXITSTATUS(status);
        if (exit_status != 0)
        {
            return 500;
        }
    }
    else
    {
        // Child process terminated abnormally
        return 500;
    }
    // std::cout << "STATUS: " << status << std::endl;
    // Read from the pipe and store the response in 'buff'

    char chunk[4096];
    ssize_t bytes_read;
    // this is blocking, should be handled by select
    close(out);
    out = open("/tmp/out", O_RDONLY);
    while ((bytes_read = read(out, chunk, sizeof(chunk))) > 0)
    {
        buff.append(chunk, bytes_read);
    }
    // Close file descriptors
    close(out);

    unlink("/tmp/out");
    this->buff = buff;
    separateHeaderFromBody(buff, this->header, this->body);
    std::string httpHeader = "HTTP/1.1 200 OK\r\n";
    this->header.insert(0, "Content-Length: " + to_string(this->body.size()) + "\r\n");
    this->header.insert(0, httpHeader);
    // std::cout << this->header << std::endl;
    return 0;
}