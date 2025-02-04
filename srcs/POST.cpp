#include "../includes/HTTPMethod.hpp"

int HTTPMethod::erase(std::string &data, size_t &chunk_size)
{
	size_t r = data.length() - chunk_size;

	if (r >= 2)
	{
		data.erase(0, chunk_size + 2);
		return 0;
	}
	else
	{
		data.erase(0, chunk_size);
		this->to_erase = true;
		return 1;
	}
	return 0;
}

int HTTPMethod::check_and_process_remaining(std::string &data, size_t &chunk_size, std::ofstream &output)
{
	if (this->to_erase)
	{
		data.erase(0, 2); // remove \r\n
		to_erase = false;
	}
	if (chunk_size > 0)
	{
		if (chunk_size > data.length())
		{
			chunk_size -= data.length();
			output << data;
			data.clear();
			return 1; // More data needs to be read
		}

		output << data.substr(0, chunk_size);
		int r = erase(data, chunk_size);
		chunk_size = 0;
		if (r)
			return 1;
	}
	return 0;
}

int HTTPMethod::process_buffered_data(std::string &data, size_t &chunk_size, std::ofstream &output)
{
	if (check_and_process_remaining(data, chunk_size, output))
		return 1; // More data needs to be read
	if (data.empty())
		return 1;
	size_t pos = data.find("\r\n");
	while (pos != std::string::npos)
	{
		std::string size = data.substr(0, pos); // to get the chunk_size as a string
		std::stringstream ss(size);
		ss << std::hex;
		ss >> chunk_size; // convert to size_t
		if (chunk_size == 0)
			return 0;					  // we reached the end of the body
		data.erase(0, size.length() + 2); // remove the size + \r\n
		if (chunk_size <= data.length())  // the whole chunk has been processed, lets look for the next /r/n
		{
			output << data.substr(0, chunk_size);
			int r = erase(data, chunk_size); // data has been processed, delete it
			chunk_size = 0;
			if (r)
				return 1;
			pos = data.find("\r\n"); // Find next chunk size
		}
		else // the chunk size was larger so we need to read again..
		{
			chunk_size -= data.length();
			output << data;
			data = "";
			break;
		}
	}
	return 1;
}

// init chunk, file
void HTTPMethod::handle_chunked_post(Client *client)
{
	client->chunk_file_name = client->file_name;
	size_t dot_position = client->chunk_file_name.find_last_of("."); // to find the type of the file

	if (dot_position != std::string::npos)
		client->chunk_file_name.insert(dot_position, "_chunked");
	else
		client->chunk_file_name += "_chunked";
	if (client->chunk_output)
		delete client->chunk_output;
	client->chunk_output = new std::ofstream(client->chunk_file_name.c_str(), std::ios::binary | std::ios::out | std::ios::app); // we create a file which will hold the parsed body
	if (!client->chunk_output || !client->chunk_output->is_open())
	{
		this->sendCodeResponse("404");
	}
	client->file_body->seekg(0, std::ios::beg); // To reset the pointer to the beginning of the file
	this->to_erase = false;

	client->has_chunked_body = true;
	client->chunk_size = 0;
	client->chunk_data = "";

	// process_chunked_data(client->file_body, output); // process the body and copy it to our file
	// client->file_body.close();	// we close the old file and deleted, as we dont need it anymore
	// output.close();
	// unlink(client->file_name.c_str());
	// client->file_name = chunked_file;
}

int HTTPMethod::POST(Client *client) // Function to handle the POST request
{
	if (this->htype == Chunk && !client->has_chunked_body)
	{
		handle_chunked_post(client);
		response_ready = true;
		return 0;
	}
	else
	{
		if (client->file_body && client->file_body->is_open())
			client->file_body->close();
	}
	int result = initializeFileHandling(client);
	if (result == 1) // error occurred and the file need to be deleted
	{
		client->delete_file();
		return 0;
	}
	else if (result == 2)
	{
		Cgi cgi(this->_Location_Scoop, Request_header);
		int res = cgi.execute_cgi(*client, this->Path, Request_header["ext"]);
		if (res == 0)
		{
			response_ready = true;
			return 0;
		}
		sendCodeResponse(to_string((size_t)res));
		this->response_ready = true;
		return 0;
	}
	this->response_ready = true;
	this->sendCodeResponse("201");
	return 0;
}

// Function to initialize file handling
int HTTPMethod::initializeFileHandling(Client *client)
{
	struct stat state;
	stat(this->Path.c_str(), &state);

	if (S_ISDIR(state.st_mode)) // is it a directory
	{
		if (this->_Location_Scoop.upload != true) // is the  directory in the right path to be upload
		{
			sendCodeResponse("403");
			return 1;
		}
	}
	else // use cgi to handle file
	{
		if (this->_Location_Scoop.cgi)
			return prepapreHeaders(client);
		else
		{
			sendCodeResponse("403");
			return 1;
		}
	}
	return 0;
}

int HTTPMethod::prepapreHeaders(Client *client)
{
	std::string extension = Path.substr(Path.find_last_of('.'));
	std::string mime_type = getMimeType(extension);
	Request_header["ext"] = extension;
	Request_header["query"] = this->query;
	Request_header["method"] = this->Method;
	Request_header["body_file"] = client->file_name;

	return 2;
}

// Function to handle Content-Length
int HTTPMethod::handleContentLength(const char *body, int bytesreceived, int &StatValue)
{
	(void)body;
	std::string bytesreceivedStr;
	bytesreceivedStr = to_string((size_t)bytesreceived);

	if (this->Request_header.find("Content-Length") == this->Request_header.end())
	{
		sendCodeResponse("411");
		return 1;
	}
	else if (this->Request_header["Content-Length"] == "0")
	{
		sendCodeResponse("400");
		return 1;
	}
	else if (this->Request_header["Content-Length"] == bytesreceivedStr)
	{
		StatValue = 1;
		return 1;
	}
	return 0;
}

// Function to close the file and send response
void HTTPMethod::closeFileAndSendResponse()
{
	this->response_ready = true;

	this->sendCodeResponse("201");
}
