#include "../includes/server.hpp"

server::server(){}

server::~server(){
    close(server_socket);
}
void server::AcceptClient(int c)
{
    
    tmp.clien_socket = accept(c,tmp.clienfo->ai_addr,&tmp.clienfo->ai_addrlen);
    if(tmp.clien_socket == -1)
    {
        std::cerr<<"unable to establish a connection to the clein " << c<< std::endl;
        return;
    }
    FD_SET(tmp.clien_socket,&fd_r);
    FD_SET(tmp.clien_socket,&fd_w);
    if(maxfd < tmp.clien_socket)
        maxfd = tmp.clien_socket;
    ClientMap.insert(std::make_pair(tmp.clien_socket,client(tmp)));
}

void server::SelectInit()
{
    time.tv_sec = 60;
    time.tv_usec = 0;
    // telling select to Return immediately after checking the descriptors
    FD_ZERO(&fd_r);
    FD_ZERO(&fd_w);
    FD_SET(server_socket,&fd_r);
    FD_SET(server_socket,&fd_w);
    maxfd = server_socket;
}
void server::ServInit()
{
    struct addrinfo address;
    memset(&address,0,sizeof(address));// make sure the struct is empty
    address.ai_family = AF_INET;// IPV4
    address.ai_socktype = SOCK_STREAM;// TCP stream sockets
    address.ai_flags = AI_PASSIVE;  // fill in my IP for me
    if(getaddrinfo(ip.c_str(),port.c_str(),&address,&servinfo) != 0)//ensure that the structure is properly initiated and all information is added correctly
        check(-1,"getaddrinfo error");
}

void server::check(int namber ,std::string error)
{
    if(namber == SOCKETERROR)
    {
        std::cerr << error << std::endl;
        exit(EXIT_FAILURE); 
    }
}

int server::CreatServer(int port, char* ip)
{
    int client_count ;
    ServInit();
    check((server_socket = socket(servinfo->ai_family,servinfo->ai_socktype,servinfo->ai_protocol)),"Socket creation failed");
    SelectInit();
    check(fcntl(server_socket,F_SETFL,O_NONBLOCK),"fcntl failed");//setting the server in a non-blocking state
    int opt = 1;
    check(setsockopt(server_socket,SOL_SOCKET,SO_REUSEPORT,&opt,sizeof(opt)),"SETSOCKOPT failed ");//fix bind error
    check(bind(server_socket,servinfo->ai_addr,servinfo->ai_addrlen),"bind failed");
    check(listen(server_socket,FD_SETSIZE),"listen failed");
    std::cout << "Server listening on port " << port << std::endl;
    while (true)
    {
        fd_tr = fd_r;
        fd_tw = fd_w;
        check(client_count = select(maxfd + 1,&fd_tr,&fd_tw,0,&time),"Select error");
        for (int i = 0; i < maxfd; i++)
        {
            if(FD_ISSET(i,&fd_tr))
            {
                if(i == server_socket)
                    AcceptClient(i);
                else
                    HandleRequest(i);
            }
            else if(FD_ISSET(i,&fd_tr))
                HandleResponse(i);
        }
        
    }
     freeaddrinfo(servinfo);
}

void server::HandleResponse(int c)
{
    int total = 0;
    int bytesend = 0;
    std::map<int,client>::iterator it = ClientMap.find(c);
    if(it == ClientMap.end())
    {
        std::cerr << " client not found \n";
        return;
    }
    int bytesleft = it->second.rtmp.size();
    while(total < it->second.rtmp.size())
    {
        check(bytesend = send(it->second.clien_socket,it->second.&rtmp[total],bytesleft,0),"send error ");
        total += bytesend;
        bytesleft -= bytesend;
    }
    
}

void server::HandleRequest(int c)
{
    std::map<int,client>::iterator it = ClientMap.find(c); 
    int bytesRead;
    std::string stream;
    if(it == ClientMap.end())
    {
        std::cerr << " client not found \n";
        return;
    }
    std::string stream;
    int bytesRead = 0;                
    while (true)
    {
        char buffer[BUFSIZE] ={0};
        bytesRead = recv(c,buffer,sizeof(buffer),0);
        if(bytesRead <= 0)// Connection closed or error
        {
            std::cout << "failed to read from \n " << c << "or Connection closed " << std::endl;
            close(c);
            FD_CLR(c,&fd_r);
            FD_CLR(c,&fd_w);
            break;
        }
        else if(stream.find("\r\n\r\n") != std::string::npos)//received the entire request
        {
            stream.push_back('\0');//sealed the stream
            it->second.rtmp = stream;
            break;
        }
        else{
            stream.append(buffer,bytesRead);
        }
    }

}