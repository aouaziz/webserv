#include "../includes/HTTP.hpp"

int HTTP::erase(std::string &data, size_t &chunk_size)
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


int HTTP::check_and_process_remaining(std::string &data, size_t &chunk_size, std::ofstream &output)
{
	if (this->to_erase)
	{
		data.erase(0, 2);
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
		std::cout << chunk_size << " : " << data.length() << std::endl;
		int r = erase(data, chunk_size);
		chunk_size = 0;
		if (r)
			return 1;
	}
	return 0;
}

int HTTP::process_buffered_data(std::string &data, size_t &chunk_size, std::ofstream &output)
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
		std::cout << size << ": " << chunk_size << std::endl;
		if (chunk_size == 0)
			return 0;	// we reached the end of the body
		  // skips both the size and \r\n
		data.erase(0, size.length() + 2);
		if (chunk_size <= data.length())
		{
			// the whole chunk has been processed, lets look for the next /r/n
			output << data.substr(0, chunk_size);
			int r = erase(data, chunk_size); // data has been processed, delete it, (2 for the /r/n after the chunk)
			chunk_size = 0;
			if (r)
				return 1;
			pos = data.find("\r\n");	   // Find next chunk size
		}
		else
		{
			// the chunk size was larger so we need to read again../
			chunk_size -= data.length();
			output << data;
			data = "";
			break ; // so we break out of the loop and read again
		}
	}
	return 1;
}

void HTTP::process_chunked_data(std::fstream &input, std::ofstream &output)
{
	size_t buffer_size = 2048;
	char buffer[2048];
	size_t chunk_size = 0;
	std::string data = "";
	size_t total_bytes_read = 0;

	while (input.read(buffer, buffer_size)) // read from input file
	{
		total_bytes_read += input.gcount();
		data.append(buffer, input.gcount());
		int res = process_buffered_data(data, chunk_size, output);
		if (res == 0)
		{
			std::cout << "BODY SIZE " << total_bytes_read << std::endl;
			return ;
		}
	}
	// Check if there's any remaining data
	if (input.gcount() > 0)
	{
		std::cout << "remaining data " << std::endl;
		data.append(buffer, input.gcount());
		int res = process_buffered_data(data, chunk_size, output);
		if (res == 1) // there is no more data to read
			return;	  // there is an error to handle later
	}
}

// init chunk, file
void HTTP::handle_chunked_post(Client *client)
{
	// we create a file which will hold the parsed body
	std::string chunked_file = client->file_name;
	size_t dot_position = chunked_file.find(".");

	if (dot_position != std::string::npos)
		chunked_file.insert(dot_position, "_chunked");
	else
		chunked_file += "_chunked";

	std::ofstream output(chunked_file.c_str(), std::ios::binary | std::ios::out | std::ios::app);
	if (!output.is_open())
	{
		// error
	}
	client->file_body.close();
	client->file_body.open(client->file_name.c_str(), std::ios::binary | std::ios::in);
	this->to_erase = false;
	process_chunked_data(client->file_body, output); // process the body and copy it to our file
	// we close the old file and deleted, as we dont need it anymore
	client->file_body.close();
	output.close();
	unlink(client->file_name.c_str());
	client->file_name = chunked_file;
}

// Function to handle the POST request
int HTTP::POST(Client *client)
{
	if (this->htype == Chunk)
		handle_chunked_post(client);
	else
		client->file_body.close();
	int result = initializeFileHandling(client);
	if (result == 1)
	{
		client->delete_file();
		return 0;
	}
	else if (result == 2)
	{
		Cgi cgi(this->_Location_Scoop, Request_header);
		std::cout << "ext " << Request_header["ext"] << std::endl;
		int res = cgi.execute_cgi(this->Path, Request_header["ext"]);
		unlink(client->file_name.c_str());
		if (res == 0)
		{
			Response.append(cgi.header);
			Response.append(cgi.body);			
			response_ready = true;
			return 0;
		}
		//sendCodeResponse(to_string((size_t)res));
		this->response_ready = true;
		return 0;
	}
	this->response_ready = true;
	//sendCodeResponse("201");
	return 0;
}


// Function to initialize file handling
int HTTP::initializeFileHandling(Client *client)
{
	struct stat state;
	stat(this->Path.c_str(), &state);

	if (S_ISDIR(state.st_mode))
	{
		if (this->_Location_Scoop.upload != true)
		{
			//sendCodeResponse("403");
			return 1;
		}
	}
	else
	{ // use cgi
		if (this->_Location_Scoop.cgi)
			return prepapreHeaders(client);
		else
		{
			//sendCodeResponse("403");
			return 1;
		}
	}
	return 0;
}

// Function to handle Content-Length
int HTTP::handleContentLength(const char *body, int bytesreceived, int &StatValue)
{
	std::string bytesreceivedStr;
	bytesreceivedStr = to_string((size_t)bytesreceived);

	if (this->Request_header.find("Content-Length") == this->Request_header.end())
	{
		//sendCodeResponse("411");
		return 1;
	}
	else if (this->Request_header["Content-Length"] == "0")
	{
		//sendCodeResponse("400");
		return 1;
	}
	else if (this->Request_header["Content-Length"] == bytesreceivedStr)
	{
		write(this->PostFd, body, bytesreceived);
		StatValue = 1;
		return 1;
	}
	return 0;
}

// Function to close the file and send response
