#include "../includes/webserv.hpp"

void webserv::Erorr(std::string error)
{
    std::cerr << error << std::endl;
    exit(EXIT_FAILURE); 
}



void webserv::Start()
{
    std::string hello = "Hello from serer\n";
    int addrlen = sizeof(address);
    if((server_socket =  socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
       throw std::runtime_error("Socket creation failed");
    }
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);//port
    address.sin_addr.s_addr = INADDR_ANY;//ip address
    if(bind(server_socket, (struct sockaddr *)&address, addrlen) < 0)
    {
        throw std::runtime_error("bind failed");     
           close(server_socket);

    }
    if(listen(server_socket, MAX_CLIENTS) < 0)
    {
        throw std::runtime_error("listen failed");
        close(server_socket);
    }
    fd_set sockets;
    FD_ZERO(&sockets);
    FD_SET(server_socket, &sockets);
    int maxfd =server_socket;
    std::cout << "Server listening on port " << PORT << std::endl;

    while (true)
    { 
        fd_set copy = sockets;  
        int socketcount = select(FD_SETSIZE,&copy,NULL,NULL,NULL);
        if(socketcount <0)
            Erorr("Select error");
        for(int i =0;i<FD_SETSIZE;i++)
        {
            if(FD_ISSET(i,&copy)) 
            {
                if(i == server_socket) // New call on listening socket
                {
                    int client_socket = accept(server_socket,NULL,NULL);
                    if(client_socket !=-1)
                    {
                        std::cout <<  "New connection established on socket " << client_socket << std::endl;
                        FD_SET(client_socket,&sockets);
                    }
                        
                }
                else{   
                    std::string stream;
                    int bytesRead = 0;                // Activity on client socket
                    while (true)
                    {
                
                
                        char buffer[BUFSIZE] ={0};
                        bytesRead = recv(i,buffer,sizeof(buffer),0);
                        if(bytesRead <= 0)// Connection closed or error
                        {
                            std::cout << "failed to read from \n " << i << "or Connection closed " << std::endl;
                            close(i);
                            FD_CLR(i,&sockets);
                            break;
                        }
                        else if(stream.find("\r\n\r\n") != std::string::npos)//received the entire request
                        {
                            stream.push_back('\0');//sealed the stream
                            break;
                        }
                        else{
                            stream.append(buffer,bytesRead);
                        }
                    }
                    if(bytesRead >=0)
                    {
                     int s = send(i, hello.c_str(), hello.size(), 0);
                        if (s < 0)
                            std::cerr << "Failed to send message" << std::endl;
                        else
                            std::cout << "Message sent successfully" << std::endl;
                    }
                    }
            }
        }
    }
    close(server_socket);
}

webserv::webserv()
{ 
}
webserv::~webserv()
{
}
