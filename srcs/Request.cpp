#include "../includes/HTTPMethod.hpp"


void HTTPMethod::removeCarriageReturns(std::string &inputString)//request file
{
	size_t found = inputString.find('\r');

	while (found != std::string::npos)// replace all '\r' with ' '
	{ 
		inputString.erase(found, 1);
		found = inputString.find('\r', found + 1);
	}
}


std::string HTTPMethod::AdjustUriWithRoot(const std::string &uri, const std::string &locationPath, const std::string &root)
{
	std::string matchedUri = uri;
	std::size_t locationPos = matchedUri.find(locationPath);
	if (locationPos != std::string::npos)
		matchedUri.replace(locationPos, locationPath.length(), root + "/");
	return matchedUri;
}

//comparePaths the path of the request with the path of the location
bool comparePaths(std::string &Path, LocationConfig &location)
{
	size_t i = 0;
	size_t size_Path = Path.size();
	size_t size_location_path = location.path.size();
	while (i < size_Path && i < size_location_path)
	{
		if (Path[i] == location.path[i])
			i++;
		else
			break;
	}
	if (i == location.path.size())
	{
		
		if (i == Path.size() || (i < Path.size() && Path[i] == '/') || (i > 0 && Path[i - 1] == '/'))//if the path of the location is a prefix of the path of the request
			return true;
	}
	
	return false;
}

std::string HTTPMethod::getMimeType(const std::string &extension)
{
	if (_linker.Mime_types.find(extension) != _linker.Mime_types.end())
		return _linker.Mime_types[extension];
	else
		return "text/plain";
}

void HTTPMethod::DetermineRequestType()//request file 
{
	std::map<std::string, std::string>::iterator it; //itorate over the request header

	if ((it = Request_header.find("Content-Length")) != Request_header.end())
	{
		htype = ContentLength;
		BodyLength = to_number(it->second.c_str());
	}
	else if ((it = Request_header.find("Transfer-Encoding")) != Request_header.end() && it->second.find("chunked") != std::string::npos)
	{
		htype = Chunk;
	}
	else
		htype = Unknown;
}

bool fn(LocationConfig &x, LocationConfig &y)
{
	return (x.path.length() > y.path.length());
}

bool HTTPMethod::CheckRequestValidity()
{
	std::string uri = this->Uri;
	const std::string NonWantedChar = " <>{}|\\^`";

	if (uri.empty())
	{
		sendCodeResponse("400");
		return true;
	}
	if (Uri.length() > 2048)
	{
		sendCodeResponse("414");
		return true;
	}
	if (uri.find_first_of(NonWantedChar) != std::string::npos)
	{
		sendCodeResponse("400");
		return true;
	}
	if (uri.find("..") != std::string::npos)
	{
		sendCodeResponse("403");
		return true;
	}
	if (Method != "GET" && Method != "POST" && Method != "DELETE")
	{
		sendCodeResponse("501");
		return true;
	}
	if (Version != "HTTP/1.1")
	{
		sendCodeResponse("505");
		return true;
	}
	if (this->Method == "POST")
	{
		if (this->htype == ContentLength && this->BodyLength == 0)
		{
			sendCodeResponse("400");
			return true;
		}
		else if (this->htype == Unknown)
		{
			sendCodeResponse("411");
			return true;
		}
	}
	return false;
}

bool HTTPMethod::RouteRequest()
{
	if(CheckRequestValidity())
		return false;
	this->Path = this->Uri;
	std::vector<LocationConfig> serverLocations = this->_config.locations;
	size_t i = 0;
	if (this->Uri.find('?') != std::string::npos)
		Path = this->Uri.substr(0, this->Uri.find('?'));
	std::sort(serverLocations.begin(), serverLocations.end(), fn);
	for (i = 0; i < serverLocations.size(); i++)
	{
		if (comparePaths(Path, serverLocations[i]))
		{
			this->_Location_Scoop = serverLocations[i];
			break;
		}
	}
	if (i == serverLocations.size())
	{
		this->sendCodeResponse("404");
		return true;
	}
	std::vector<std::string> methods_of_location = this->_Location_Scoop.methods;
	std::vector<std::string>::iterator it;
	bool methodFound = false;
	for (it = methods_of_location.begin(); it != methods_of_location.end(); ++it)
	{
		if (*it == this->Method)
		{
			methodFound = true;
			break;
		}
	}
	if (!methodFound)
	{
		this->sendCodeResponse("405");
		return true;
	}
	Path = AdjustUriWithRoot(this->Path, this->_Location_Scoop.path, this->_Location_Scoop.root);
	return false;
}


int HTTPMethod::parseRequestHeader(std::string req)//request file
{
	std::vector<std::string> lines;
	std::istringstream iss(req);
	std::string line;
	while (std::getline(iss, line))
	{
		removeCarriageReturns(line);
		lines.push_back(line);
	}

	std::istringstream first_line_iss(lines[0]);

	first_line_iss >> this->Method >> this->Uri >> this->Version;
	size_t pos = this->Uri.find("?");
	if (pos != std::string::npos)
	{
		this->query = this->Uri.substr(pos + 1);
		this->Uri = this->Uri.substr(0, pos);
	}
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
	}
	DetermineRequestType();
	if (this->RouteRequest() || htype == Unknown)
		return 1;
	return 0;
}
