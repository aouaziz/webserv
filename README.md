<h1 align="center" id="title">Webserv</h1>

<p align="center"><img src="https://github.com/aouaziz/webserv/blob/master/Data/68747470733a2f2f692e696d6775722e636f6d2f69336e734d78432e6a7067.jpeg" alt="project-image"></p>

<p id="description">The goal of The aim of this project is to create an HTTP web server using C++98 from scratch. The web server must be capable of handling various HTTP requests such as GET HEAD POST PUT and DELETE and must have the ability to serve static files from a specific root directory or dynamic content using CGI. Additionally it should be able to handle multiple client connections concurrently with the aid of select().project is to build a C++98-compatible HTTP web server from scratch. The web server can handle HTTP GET HEAD POST PUT and DELETE Requests and can serve static files from a specified root directory or dynamic content using CGI. It is also able to handle multiple client connections concurrently with the help of select().</p>

# Sockets :

A socket is a fundamental concept in computer networking that facilitates communication between different processes running on separate devices within a network. Here are some key points to understand about sockets:

- Definition:
 A socket is often described as "a pipe between two computers on a network through which data flows" (Mulholland, 2004). This definition emphasizes the role of a socket as a communication channel that allows data to be exchanged between two devices.

- Winsock Functions:
 In the context of Windows programming, most Winsock functions operate on a socket. Winsock (Windows Sockets) is a programming interface that enables software applications to communicate over a network. The socket, in this context, serves as a handle for the connection. Both the sending and receiving ends of a connection use a socket to manage the communication.

- Two-Way Communication:
 Sockets facilitate two-way communication, meaning that data can be both sent and received on a socket. This bidirectional nature allows for real-time interaction between processes running on different devices.

- Types of Sockets:

Streaming Socket (SOCK_STREAM): This type of socket uses TCP (Transmission Control Protocol) for communication. TCP provides a reliable, connection-oriented communication stream, making it suitable for applications where data integrity and order are crucial, such as file transfers or web browsing.
Datagram Socket (SOCK_DGRAM): This type of socket uses UDP (User Datagram Protocol), which is a connectionless protocol. Datagram sockets are often used in scenarios where low latency and quick transmission of data are more important than ensuring the order and reliability of data, such as real-time video streaming or online gaming.
In summary, sockets are essential for enabling communication between processes on different devices in a network. They provide a versatile mechanism for data exchange, supporting both reliable, connection-oriented communication (TCP) and faster, connectionless communication (UDP). Sockets are a fundamental building block for various networked applications, including web browsers, file transfer protocols, and online games.



