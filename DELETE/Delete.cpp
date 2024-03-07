#include <iostream>
#include <cstring>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fstream>
#include <fcntl.h>

bool remove_folder(const std::string & path)
{
    DIR *d = opendir(path.c_str());
    if (d  == NULL) {
        std::cerr << "Error opening directory: " << path << std::endl;
        return false;
    }
    struct dirent *p;//dirent is a struct that contains the name of the file and the type of the file we use it to loop through the directory
    while((p = readdir(d)) != NULL) //readdir returns a pointer to the next dirent struct in the directory
    {
        if (strcmp(p->d_name, ".") != 0 && strcmp(p->d_name, "..") != 0) //we don't want to delete the current directory or the parent directory so we skip them d_name is the name of the file in the directory 
        {
            std::string file_path = path + "/" + p->d_name;//we create the path of the file we want to delete
            if (p->d_type == DT_DIR) {//if the file is a directory we call the function recursively
                remove_folder(file_path);
            }
            else 
            {
                if (unlink(file_path.c_str()) == -1)//if the file is not a directory we delete it
                {
                    std::cerr << "Error deleting file: " << file_path << std::endl;
                    return false;
                } 
            }
        }
    }
    closedir(d);//we close the directory
    if (rmdir(path.c_str()) == -1)//we try to delete the directory
    {
        std::cerr << "Error deleting directory: " << path << std::endl;
        return false;
    }
    return true;
}

int is_directory(const std::string & path)//check if the path is a directory
{
    struct stat s;
    if (stat(path.c_str(), &s) !=0 )
    {
        std::cerr << "Error: stat failed" << std::endl;
        return -1;
    }
    return S_ISDIR(s.st_mode);//S_ISDIR is a macro that returns true if the file is a directory
}

int DELETE(const std::string & path)//check if the file exists if it does not exist return 404
{
    if (access(path.c_str(), F_OK) == -1)
    {
        std::cerr << "Error: file does not exist" << std::endl;
        return 404; //NOT FOUND
    }
    if (is_directory(path) == 1)//if the path is a directory we delete it
    {
        //check if the directory ended with a slash 
        if (path[path.length() - 1] != '/')
            return 409; // CONFLICT
        if (access(path.c_str(), W_OK) == -1)
            return 403; // FORBIDDEN
        if (remove_folder(path) == true)
            return 204; // NO CONTENT
        else 
            return 500; // INTERNAL SERVER ERROR
    }
    else
    {
        if (unlink(path.c_str()) == -1)//if the path is a file we delete it
        {
            std::cerr << "Error deleting file: " << path << std::endl;
            return 500;
        }
    }
}

int main()
{
    // bool cgi_on = true;
    std::string path = "./test.php";
    // METHOD GET

    int f_w = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (f_w == -1)
    {
        std::cerr << "Error: open failed" << std::endl;
        return 1;
    }
    

    std::string cgi_path = "./../php-cgi";
    std::string cgi_arg = path;
    std::string cgi_env2 = "CONTENT_LENGTH=304";
    std::string cgi_env3 = "CONTENT_TYPE=text/html";
    std::string cgi_env4 = "GATEWAY_INTERFACE=CGI/1.1";
    std::string cgi_env7 = "QUERY_STRING=";
    std::string cgi_env1 = "REQUEST_METHOD=GET";


    int fd[2];
    pipe(fd);
    pid_t pid = fork();
    if (pid == 0)
    {
        char *argv[3] = {(char*)cgi_path.c_str(), (char*)cgi_arg.c_str(), NULL};
        char *envp[] = {const_cast<char *>(cgi_env1.c_str()), const_cast<char *>(cgi_env2.c_str()), const_cast<char *>(cgi_env3.c_str()), const_cast<char *>(cgi_env4.c_str()), const_cast<char *>(cgi_env7.c_str()), NULL};
        dup2(fd[1], 1);
        close(1);
        dup2(f_w, 0);
        execve(argv[0], argv, envp);
        exit(0);
    }
    close(fd[1]);
    char buffer[4096];
    int n = read(fd[0], buffer, 4096);
    buffer[n] = '\0';
    std::cout << buffer << std::endl;
    close(fd[0]);
    


        
    


    // remove_folder("text");
}