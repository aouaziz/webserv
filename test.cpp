void    Error(const char *msg)
{
    perror(msg);
    exit(1);
}

void    Server::Init()
{
    memset(&server_infos, 0, sizeof(server_infos));
    server_infos.ai_family      = AF_INET;
    server_infos.ai_protocol    = SOCK_STREAM;
    getaddrinfo(LOCALHOST, PORT, &server_infos, &sinfo_ptr);
}

void    Server::CreateServer()
{
    Init();
    if ((server_socket = socket(sinfo_ptr->ai_family, sinfo_ptr->ai_protocol, 0)) == -1)
       Error("Error: Creating socket failed\n");
    int optval = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
    if (bind(server_socket, sinfo_ptr->ai_addr, sinfo_ptr->ai_addrlen) == -1)
       Error("Error: Binding failed\n");
    if (listen(server_socket, FD_SETSIZE) == -1)
       Error("Error: Listening failed\n");
}

void Server::SendResponseHeader(int clt_skt)
{
    char response_header[] =    "HTTP/1.1 200 OK\r\n"
                                "Server: Allah Y7ssen L3wan\r\n"
                                "Content-Length: 82013359\r\n"
                                "Content-Type: video/mp4\r\n\r\n";
    if (send(clt_skt, response_header, strlen(response_header), 0) == -1)
        Error("Error (Send) -> ");
}

void Server::DropClient()
{
    close(active_clt);
    FD_CLR(active_clt, &readfds);
    FD_CLR(active_clt, &writefds);
    _clients.erase(itb);
    close(itb->GetCltFd());
    std::cerr << "Connection Closed\n";
}


int Server::AcceptAddClientToSet()
{
    int newconnection = accept(server_socket, (struct sockaddr *)&storage_sock, &clt_addr);
    if (newconnection == -1)
        Error("Error (Accept) -> ");
    const char *path = "/Users/me/Desktop/video.mp4";
    _clients.push_back(Client(newconnection, open(path, O_RDONLY)));
    client_write_ready = false;
    FD_SET(_clients.back().GetCltSocket(), &readfds);
    FD_SET(_clients.back().GetCltSocket(), &writefds);
    if (_clients.back().GetCltSocket() > maxfds)
        maxfds = _clients.back().GetCltSocket();
    return (newconnection);
}

void Server::Start()
{
    CreateServer();
    
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_SET(server_socket, &readfds);
    FD_SET(server_socket, &writefds);
    maxfds = server_socket;
    bytesreceived = 0;

    while (TRUE)
    {
        tmpfdsread = readfds;
        tmpfdswrite = writefds;
        activity = select(maxfds + 1, &tmpfdsread, &tmpfdswrite, NULL, NULL);
        if (activity == -1)
           Error("Error (Select) -> ");
        if (FD_ISSET(server_socket, &tmpfdsread))
            client_socket = AcceptAddClientToSet();
        for (itb = _clients.begin(); itb != _clients.end(); itb++)
        {
            active_clt = itb->GetCltSocket();
            if (FD_ISSET(active_clt, &tmpfdsread) && !client_write_ready)
            {
                bytesreceived = recv(active_clt, requested_data, sizeof(requested_data), 0);
                if (bytesreceived <= 0)
                {
                    DropClient();
                    std::cerr << "Connection Closed\n";
                }
                else
                {
                    client_write_ready = true;
                    SendResponseHeader(active_clt);
                }
            }

            if (FD_ISSET(active_clt, &tmpfdswrite) && client_write_ready)
            {
                bytesread = read(itb->GetCltFd(), buffer, sizeof(buffer));
                if (bytesread == -1)
                    Error("Error (Read) -> ");
                bytessent = send(active_clt, buffer, bytesread, 0);
                if (bytessent == -1)
                {
                    std::cout << "Cannot Send\n";
                        Error("Error (Send) -> ");
                }
                if (bytesread == 0)
                {
                    DropClient();
                    std::cout << "Done With This Client ->" <<  itb->GetCltFd() << "\n";
                }
            }
        }
    }
    close(server_socket);
}