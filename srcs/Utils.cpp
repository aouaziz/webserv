#include "../includes/HTTPMethod.hpp"

// Check for any any illegal characters in the URI
bool HTTPMethod::ValidateURI(const std::string &uri)
{
	// Check if the URI starts with a leading slash ("/")
	if (uri.empty() || uri[0] != '/')
		return false;

	// Check for any invalid characters in the URI
	const std::string invalidChars = " <>{}|\\^`";
	if (uri.find_first_of(invalidChars) != std::string::npos)
		return false;
	return true;
}

// Check for Possible error in the Request
bool HTTPMethod::ValidateRequest() {
	// if Transfer-Encoding exist and not match [chunked]
	// if (this->Request_header.find("Transfer-Encoding") != this->Request_header.end() && this->Request_header["Transfer-Encoding"] != "chunked")
	// {
	// 	this->sendCodeResponse("501");
	// 	return false;
	// }
	// // if both Transfer-Encoding and Content-Length not provided
	// if (this->Method == "POST" && this->Request_header.find("Transfer-Encoding") == this->Request_header.end() && this->Request_header.find("Content-Length") == this->Request_header.end())
	// {
	// 	this->sendCodeResponse("400");
	// 	return false;
	// }
	// URI should start with a leading slash ("/") and not contain any illegal characters
	if (!this->ValidateURI(this->Uri))
	{
		this->sendCodeResponse("400");
		return false;
	}
	//  URI should not have more than 2048
	if (this->Uri.length() > 2048)
	{
		this->sendCodeResponse("414");
		return false;
	}
	// //  Request body size should not be more than [client_body_size] from confing file
	// if (this->Request_header.find("Content-Length") != this->Request_header.end() && std::stoll(this->Request_header["Content-Length"]) > std::stoll(this->_config.common.client_max_body_size))
	// {
	// 	this->sendCodeResponse("413");
	// 	return false;
	// }
	return this->MatchLocation();
}


// int HTTPMethod::parseRequestHeader(char *req, int bytesreceived) // ß
// {
// 	int delimiter_position;
// 	char *body;
// 	std::string current_line, key, value;
// 	std::string request = req;
// 	size_t header_len = request.find("\r\n\r\n"); // Find the end of the request header
// 	if (header_len == std::string::npos)
// 	{
// 		perror("Error : ParseRequstHeader <Finding end of request> ");
// 		this->sendCodeResponse("500");
// 		return 0;
// 	}
// 	std::string header = request.substr(0, header_len);			  // Save the header
// 	body = req + header_len + 4, bytesreceived -= header_len + 4; // Save the body
// 	std::stringstream request_stream(header);
// 	request_stream >> std::skipws >> std::ws >> this->Method; // Streaming methode into Methode while taking care of white spaces
// 	request_stream >> std::skipws >> std::ws >> this->Uri;	   // same for path
// 	request_stream >> std::skipws >> std::ws >> this->Version; // same for versionß
// 	std::getline(request_stream, current_line);				   // skip the remaining part of the request line (HTTP/1.1)
// 	while (getline(request_stream >> std::ws >> std::skipws, current_line, '\n')) // read the rest of the header line by line
// 	{
// 		current_line.erase(std::remove(current_line.begin(), current_line.end(), '\r'), current_line.end()); // remove every occurence of '/r' in line
// 		delimiter_position = current_line.find(':');
// 		key = current_line.substr(0, delimiter_position);
// 		value = current_line.substr(delimiter_position + 2, current_line.length()); // [delimiter_position + 2] to remove the extra space before value
// 		// storing key and value in map
// 		this->Request_header[key] = value;
// 	}
// 	/* DUBG
// 	this->HeaderState = 1;
// 	return 1;
// }
// int HTTPMethod::parseRequestHeader(char *req, int bytesreceived) // 
// {
//     int delimiter_position;
// 	// char *body;
//     std::string current_line, key, value;
//     std::string request = req;
//     size_t header_len = request.find("\r\n\r\n"); // Find the end of the request header
//     if (header_len == std::string::npos)
//     {
//         perror("Error : ParseRequstHeader <Finding end of request> ");
//         this->sendCodeResponse("500");
//         return 0;
//     }
//     std::string header = request.substr(0, header_len); // Save the header
// 	BodyPassed = req + header_len + 4, bytesreceived -= header_len + 4; // Save the body
//     std::stringstream request_stream(header);
//     request_stream >> std::skipws >> std::ws >> this->Method; // Streaming methode into Methode while taking care of white spaces
//     request_stream >> std::skipws >> std::ws >> this->Uri;    // same for path
//     request_stream >> std::skipws >> std::ws >> this->Version; // same for versionß
//     std::getline(request_stream, current_line); // skip the remaining part of the request line (HTTP/1.1)
//     while (getline(request_stream >> std::ws >> std::skipws, current_line, '\n')) // read the rest of the header line by line
//     {
//         current_line.erase(std::remove(current_line.begin(), current_line.end(), '\r'), current_line.end()); // remove every occurrence of '/r' in line
//         delimiter_position = current_line.find(':');
//         key = current_line.substr(0, delimiter_position);
//         value = current_line.substr(delimiter_position + 2, current_line.length()); // [delimiter_position + 2] to remove the extra space before value
//         this->Request_header[key] = value;
//     }
// 	if (!this->ValidateRequest()) 
// 		return 0;
// 	if (this->Method == "GET")
// 		return this->GET();
// 	else if (this->Method == "POST")
// 		return this->POST(BodyPassed, bytesreceived);
// 	return 0;
// }
// int HTTPMethod::check_recv_body(int bytesreceived)
// {
// 	static long long recbytes = 0;
// 	if (std::stoll(this->Request_header["Content-Length"]) > recbytes)
// 	{	
// 		std::cout << "HERREEEE" << std::endl;
// 		recbytes += bytesreceived;
// 		this->BodyLength = recbytes;
// 		return 0;
// 	}
// 	if (std::stoll(this->Request_header["Content-Length"]) == recbytes)
// 	{
// 		if (!this->ValidateRequest()) 
// 			return 0;
// 		if (this->Method == "GET") //
// 			return (this->GET());
// 		else if (this->Method == "POST")
// 			return this->POST(this->Body, bytesreceived);
// 	}
// 	else 
// 	{
// 		this->sendCodeResponse("400");
// 		return 0;
// 	}
// 	return 0;
// }