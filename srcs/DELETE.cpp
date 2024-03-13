#include "../includes/HTTP.hpp"

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

int HTTP::DELETE(const std::string & path)//check if the file exists if it does not exist return 404
{
    int result = 0;
    if (access(path.c_str(), F_OK) == -1)
    {
        std::cerr << "Error: file does not exist" << std::endl;
        result = 404; //NOT FOUND
    }
    else if (is_directory(path) == 1)//if the path is a directory we delete it
    {
        //check if the directory ended with a slash 
        if (path[path.length() - 1] != '/')
            result = 409; // CONFLICT
        else if (access(path.c_str(), W_OK) == -1)
            result = 403; // FORBIDDEN
        else if (remove_folder(path) == true)
            result = 204; // NO CONTENT
        else 
            result = 500; // INTERNAL SERVER ERROR
    }
    else
    {
        if (unlink(path.c_str()) == -1)//if the path is a file we delete it
        {
            std::cerr << "Error deleting file: " << path << std::endl;
            result = 500;
        }
        else
            result = 204;
    }
    //sendCodeResponse(to_string(result));
    this->response_ready = true;
    return 0;
}