
#include "includes/client.hpp"

client::client()
{
    memset(&clienfo,0,sizeof(clienfo));// make sure the struct is empty
    clienfo.ai_family = AF_INET;// IPV4
    clienfo.ai_socktype = SOCK_STREAM;// TCP stream sockets
    clienfo.ai_flags = AI_PASSIVE; 
}
client::client(const client &other)
{
    *this = other;
}
client& client::operator=(const client& other) 
{
    clienfo = other.clienfo;
    this->clien_socket = other.clien_socket;
    this->request = other.request;
}
client::~client()
{
}