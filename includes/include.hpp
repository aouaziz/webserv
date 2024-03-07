#ifndef INCLUDE_HPP
#define INCLUDE_HPP


#include <set>
#include <utility>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <dirent.h>
#include <ios>
#include <ctime>
#include <algorithm>
#include <iomanip>
#include <sys/wait.h>
#include <algorithm>

#define SOCKETERROR -1
#define BUFSIZE 10000
#define MapOf_Str_Str std::map<std::string, std::string>
std::string to_string(size_t num);
size_t to_namber(const char *num);



#endif