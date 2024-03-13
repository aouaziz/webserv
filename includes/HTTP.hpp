#ifndef HTTP_HPP
#define HTTP_HPP
#include "include.hpp"
#include "Config.hpp"
#include "Cgi.hpp"
#include "Client.hpp"

enum ContentType
{
	Chunk = 1,
	ContentLength,
	Unknown
};

class Client;

class HTTP // HTTP
{
public:
	int htype;
	ServerConfig _config;
	LocationConfig _Location_Scoop;

	std::string Path;
	std::string Url;
	std::string query;
	std::string Method;
	std::string Version;
	char *Body;
	MapOf_Str_Str Request_header; // use
	MapOf_Str_Str Req_header;
	std::string Response;
	int HeaderState;
	bool response_ready;
	bool valid_header;
	char req_data[BUFSIZE];
	int client_socket;
	char *BodyPassed;
	std::string boundary;
	size_t BodyLength;
	int PostFd;
	int HexaChunkStatus;
	int ChunkSize;
	int AmountRecv;
	std::string ChunkHexStr;
	std::string PostPath;
	std::string MimeType;
	int requested_file_fd;
	bool is_header_sent;

	bool	to_erase;

	// METHODS
	HTTP(ServerConfig serverConfig);
	~HTTP();

	//DELETE
	int DELETE(const std::string &path);
	// GET FUNCTION
	int GET();
	// POST METHODS
	int handleContentLength(const char *body, int bytesreceived, int &StatValue);
	int initializeFileHandling(Client *client);
	int POST(Client *client);
	int prepapreHeaders(Client *client);
	void handle_chunked_post(Client *client);
	void process_chunked_data(std::fstream &input, std::ofstream &output);
	int process_buffered_data(std::string &data, size_t &chunk_size, std::ofstream &output);
	int check_and_process_remaining(std::string &data, size_t &chunk_size, std::ofstream &output);
	int erase(std::string &data, size_t &chunk_size);
	void ResetMethod();
	//Request
	int Request(const std::string& req);
	void replaceCarriageReturn(std::string &inputString);
	void StorRequestFirstLine(const std::string& line);
	void StorRequestHeaders(const std::vector<std::string>& lines);
	void RequestType();
	bool CheckRequestForm();
	bool RequestMethod();
};

#endif