
#include "../includes/client.hpp"

client::client()
{
    memset(&clienfo,0,sizeof(clienfo));// make sure the struct is empty
    clienfo->ai_family = AF_INET;// IPV4
    clienfo->ai_socktype = SOCK_STREAM;// TCP stream sockets
    clienfo->ai_flags = AI_PASSIVE; 
}

client::~client()
{
}