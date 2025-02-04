#ifndef HTTPMETHOD_HPP
#define HTTPMETHOD_HPP

#include "include.hpp"
#include "Config.hpp"
#include "Linker.hpp"
#include "Cgi.hpp"
#include "Client.hpp"

enum ContentType
{
	Chunk = 1,
	ContentLength,
	Unknown
};

class Client;

class HTTPMethod // HTTPMethod
{
public:
	int htype;
	ServerConfig _config;
	LocationConfig _Location_Scoop;
	Linker _linker;

	std::string Path;
	std::string Uri;
	std::string query;
	std::string Method;
	std::string Version;
	char *Body;
	MapOf_Str_Str Request_header; // use
	std::string Response;
	int HeaderState;
	bool response_ready;
	char req_data[BUFSIZE];
	int client_socket;
	char *BodyPassed;
	std::string boundary;
	size_t BodyLength;
	std::string MimeType;
	int requested_file_fd;
	bool is_header_sent;

	bool to_erase;

	// METHODS
	HTTPMethod(ServerConfig serverConfig);
	HTTPMethod(const HTTPMethod &src);
	HTTPMethod &operator=(const HTTPMethod &rhs);
	~HTTPMethod();
	void DetermineRequestType();
	void removeCarriageReturns(std::string &inputString);
	int parseRequestHeader(std::string req);
	bool CheckRequestValidity();
	bool RouteRequest();
	std::string AdjustUriWithRoot(const std::string &uri, const std::string &locationPath, const std::string &root);
	void composeResponseHeader(std::string statusCode, std::string contentType, std::string location, int contentLength);
	void sendCodeResponse(std::string CodeToSend);

	// GET FUNCTION
	int GET(Client &client);
	void handleRequestedPath(int &IndexFound);
	void serveIndexFile(int &IndexFound);
	void handleFileRequest(Client &client);
	std::string getMimeType(const std::string &extension);
	void serveAutoIndex(const std::string &directoryPath);
	// POST METHODS
	void closeFileAndSendResponse();
	int handleContentLength(const char *body, int bytesreceived, int &StatValue); // where to put the function
	int initializeFileHandling(Client *client);
	int POST(Client *client);
	int prepapreHeaders(Client *client);

	int DELETE(const std::string &path);
	void handle_chunked_post(Client *client);
	int process_buffered_data(std::string &data, size_t &chunk_size, std::ofstream &output);
	int check_and_process_remaining(std::string &data, size_t &chunk_size, std::ofstream &output);
	int erase(std::string &data, size_t &chunk_size);
};

#endif