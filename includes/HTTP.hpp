#pragma once

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

class HTTP // HTTP
{
public:
	int htype;
	ServerConfig _config;
	LocationConfig _Location_Scoop;
	Linker _linker;

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
	void RequestType();
	void replaceCarriageReturn(std::string &inputString);
	std::string GetRoot(const std::string &Url, const std::string &locationPath, const std::string &root);
	void SendResponseHeader(std::string statusCode, std::string fileExt, std::string location, int contentLength);
	void sendCodeResponse(std::string CodeToSend);
	std::string GenerateDirectoryList(std::string statusCode, std::string ls);

	// GET FUNCTION
	int GET();
	void handleDirectoryRequest(int &IndexFound);
	void handleIndexFileRequest(int &IndexFound);
	void handleAutoIndexRequest(const std::string &IfDir);
	void handleFileRequest();
	std::string getMimeType(const std::string &extension);
	void handleIndexFileExistence(std::ifstream &file);
	// POST METHODS
	void closeFileAndSendResponse();
	int handleContentLength(const char *body, int bytesreceived, int &StatValue);
	int initializeFileHandling(Client *client);
	int POST(Client *client);
	int prepapreHeaders(Client *client);

	int DELETE(const std::string &path);
	void handle_chunked_post(Client *client);
	void process_chunked_data(std::fstream &input, std::ofstream &output);
	int process_buffered_data(std::string &data, size_t &chunk_size, std::ofstream &output);
	int check_and_process_remaining(std::string &data, size_t &chunk_size, std::ofstream &output);
	int erase(std::string &data, size_t &chunk_size);
	void ResetMethod();
	bool compare(std::string &Path, LocationConfig &location);
	//Request
	int Request(const std::string& req);
	void StorRequestFirstLine(const std::string& line);
	void StorRequestHeaders(const std::vector<std::string> &lines);
	bool CheckRequestForm();
	bool CheckRequestHeaders();
	bool CheckUrlState();
	bool RequestMethod();
	bool IsMethodAllowedInLocation();
};
