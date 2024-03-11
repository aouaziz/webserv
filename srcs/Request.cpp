#include "../includes/HTTP.hpp"

bool fn(LocationConfig &x, LocationConfig &y)
{
	return (x.path.length() > y.path.length());
}

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

bool HTTP::CheckRequestForm()
{
	if(RequestMethod() || CheckUrlState() || IsMethodAllowedInLocation())
		return true;
	return false;
}

bool HTTP::RequestMethod()
{
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

bool HTTP::CheckUrlState()
{
	const std::string NonWantedChar = " <>{}|\\^`";
	if(Url.empty() || Url[0] != '/' || Url.find_first_of(NonWantedChar) != std::string::npos)
	{
		sendCodeResponse("400");
		return true;
	}
	if (Url.find("..") != std::string::npos)
	{
		sendCodeResponse("403");
		return true;
	}
	if (Url.length() > 2048)
	{
		sendCodeResponse("414");
		return true;
	}
	this->Path = this->Url;// need to be explained 
	std::vector<LocationConfig> serverLocations = this->_config.locations;
	size_t i = 0;
	std::sort(serverLocations.begin(), serverLocations.end(), fn);
	for (i = 0; i < serverLocations.size(); i++)
	{
		if (compare(Path, serverLocations[i]))
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
	return false;
}

bool HTTP::IsMethodAllowedInLocation()
{
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
	Path = GetRoot(this->Path, this->_Location_Scoop.path, this->_Location_Scoop.root);
	return false;
}

std::string HTTP::GetRoot(const std::string &Url, const std::string &locationPath, const std::string &root)
{
	std::string matchedUri = Url;
	std::string newroot =root;
	std::size_t locationPos = matchedUri.find(locationPath);
	if(newroot[newroot.size() -1] != '/')
		newroot = newroot + "/";
	if (locationPos != std::string::npos)
		matchedUri.replace(locationPos, locationPath.length(), newroot);
	return matchedUri;
}

