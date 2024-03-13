#include "../includes/HTTP.hpp"


int HTTP::Request(const std::string& req)
{
  std::vector<std::string> lines;
	std::istringstream iss(req);
	std::string line;
	while (std::getline(iss, line))
	{
		replaceCarriageReturn(line);
		lines.push_back(line);
	}
	StorRequestFirstLine(lines[0]);
	StorRequestHeaders(lines);
	if(CheckRequestForm() || Method == "GET" || Method == "DELETE")// Check if the Request is finish
		return 1;
	return 0;
}

void HTTP::replaceCarriageReturn(std::string &inputString)
{
	size_t found = inputString.find('\r');

	while (found != std::string::npos)
	{ 
		inputString.erase(found, 1);
		found = inputString.find('\r', found + 1);
	}
}

void HTTP::StorRequestFirstLine(const std::string& line) {
  std::istringstream iss(line);
  iss >> Method >> Url >> Version;
  size_t pos = this->Url.find("?");//query
	if (pos != std::string::npos)
	{
		this->query = this->Url.substr(pos + 1);
		this->Url = this->Url.substr(0, pos);
	}
}

void HTTP::StorRequestHeaders(const std::vector<std::string>& lines)
{
  for (size_t i = 1; i < lines.size(); i++)
	{
		std::string header_line = lines[i];
		size_t colon_pos = header_line.find(':');
		if (colon_pos != std::string::npos)
		{
			std::string key = header_line.substr(0, colon_pos);
			std::string value = header_line.substr(colon_pos + 2); // +2 to skip ':' and space
			Request_header.insert(std::make_pair(key, value));
		}
    else
      valid_header = true;
	}
  	RequestType();
}
void HTTP::RequestType()
{
	std::map<std::string, std::string>::iterator it;
	if ((it = Request_header.find("Transfer-Encoding")) != Request_header.end() && it->second.find("chunked") != std::string::npos)
	{
		htype = Chunk;
	}
	else if ((it = Request_header.find("Content-Length")) != Request_header.end())
	{
		htype = ContentLength;
		BodyLength = to_namber(it->second.c_str());
	}
	else
		htype = Unknown;
}

bool HTTP::CheckRequestForm()
{
	if(RequestMethod())
		return true;
	
	return false;
}

bool HTTP::RequestMethod()
{
	if (Method != "GET" && Method != "POST" && Method != "DELETE")
	{
		//sendCodeResponse("501");
		return true;
	}
	if (Version != "HTTP/1.1")
	{
		//sendCodeResponse("505");
		return true;
	}
	if (this->Method == "POST")
	{
		if (this->htype == ContentLength && this->BodyLength == 0)
		{
			//sendCodeResponse("400");
			return true;
		}
		else if (this->htype == Unknown)
		{
			//sendCodeResponse("411");
			return true;
		}
	}
	return false;
}



// bool HTTP::CheckUrlState()
// {
// 	const std::string NonWantedChar = " <>{}|\\^`";
// 	if(Url.empty() || Url[0] != '/' || Url.find_first_of(NonWantedChar) != std::string::npos)
// 	{
// 		//sendCodeResponse("400");
// 		return true;
// 	}
// 	if (Url.find("..") != std::string::npos)
// 	{
// 		//sendCodeResponse("403");
// 		return true;
// 	}
// 	if (Url.length() > 2048)
// 	{
// 		//sendCodeResponse("414");
// 		return true;
// 	}
// 	this->Path = this->Url;// need to be explained 
// 	std::vector<LocationConfig> serverLocations = this->_config.locations;
// 	size_t i = 0;
// 	std::sort(serverLocations.begin(), serverLocations.end(), fn);
// 	for (i = 0; i < serverLocations.size(); i++)
// 	{
// 		{
// 			this->_Location_Scoop = serverLocations[i];
// 			break;
// 		}
// 	}
// 	if (i == serverLocations.size())
// 	{
// 		//sendCodeResponse("404");
// 		return true;
// 	}
// 	return false;
// }



