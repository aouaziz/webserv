#include "../includes/Config.hpp"

Config::Config(): file_name("") {}


const std::vector<ServerConfig> & Config::get_servers() const
{
    return servers;
}


Config& Config::operator=(const Config& object)
{
    this->file_name = object.file_name;
    this->server_brackets = object.server_brackets;
    this->location_brackets = object.location_brackets;
    this->last_key = object.last_key;
    this->key = object.key;
    this->current_server = object.current_server;
    this->current_location = object.current_location;
    return(*this);
}

Config::Config(std::string file_name) : file_name(file_name)
{
    // std::cout << "Config constructor called" << std::endl;
    server_brackets = false;
    location_brackets = false;
}

Config::~Config()
{
    // std::cout << "Config destructor called" << std::endl;
}

void Config::handle_server(std::stringstream &ss)
{
    (void)ss;
    if(server_brackets)//in case there is a server inside a server
    {
        std::cout << "Error: server brackets not closed" ;
        return ;
    }
    current_server = ServerConfig();//reset current_server
    
}

void Config::handle_openBracket(std::stringstream &ss)
{
    std::string tmp;
    ss >> tmp;
    if (last_key == "server")
    {
        server_brackets = true;
        return ;
    }
    if (last_key == "location")
    {
        location_brackets = true;
        return ;
    }
    else
       throw std::runtime_error("Error: unknown key open bracket");
}

void Config::handle_closedBracket(std::stringstream &ss)
{
    std::string tmp;
    ss >> tmp;
    if  (location_brackets == true)
    {
        location_brackets = false;
        current_server.locations.push_back(current_location);
        return ;
    }
    if (server_brackets == true)
    {
        server_brackets = false;
        servers.push_back(current_server);
        return ;
    }
    else
        throw std::runtime_error("Error: unknown key closed bracket");
}

void Config::handle_listen(std::stringstream &ss)
{
    if (server_brackets == false || location_brackets == true)
    {
        throw std::runtime_error ("Error: listen must be inside server brackets" );
        return ;
    }
    ss >> current_server.port;//listen + port 8080 
    if(current_server.port.empty())
        throw std::runtime_error ("Error: no port specified" );
    for (size_t i = 0; i < ports.size(); i++)
    {
        if(ports[i] == current_server.port)
        {
            std::cerr << "Error: Multiple servers have the same port: " << current_server.port << std::endl;
            exit(1);
        }
    }
    ports.push_back(current_server.port);
}

void Config::handle_host(std::stringstream &ss )
{
    if (server_brackets == false || location_brackets == true)
    {
        throw std::runtime_error ("Error: host must be inside server brackets" );
        return ;
    }
    ss >> current_server.host;
    if(current_server.host.empty())
        throw std::runtime_error ("Error: no host specified" );
}

void Config::handle_server_name(std::stringstream &ss)
{  
    //server is  openn and location is closed to make sure server name is inside server brackets and not location brackets
    if (server_brackets == false  || location_brackets == true)
    {
        throw std::runtime_error ("Error: server_name must be inside server brackets" );
        return ;
    }
    ss >> current_server.server_name;
    if(current_server.server_name.empty())
        throw std::runtime_error ("Error: no server_name specified" );
}
void Config::handle_errorPages(std::stringstream &ss) {
    if (!location_brackets && !server_brackets) {
        throw std::runtime_error("Error: error_pages must be inside location or server brackets");
    }

    int error_code;
    if (!(ss >> error_code)) {
        throw std::runtime_error("Error: error_pages must be followed by a code");
    }

    std::string error_page;
    ss >> error_page;

    std::string error_code_str;  // Store the error code as a string

    std::stringstream ss_code;
    
    ss_code << error_code;
    error_code_str = ss_code.str();
    
    if (location_brackets) {
        current_location.error_pages[error_code_str] = error_page;
    } else if (server_brackets) {
        current_server.common.error_pages[error_code_str] = error_page;
    }
}


void Config::handle_maxBodySize(std::stringstream &ss)
{
    if (location_brackets == false && server_brackets == false)
    {
        throw std::runtime_error ("Error: client_max_body_size must be inside location or server brackets" );
        return ;
    }
    std::string max_body_size;
    ss >> max_body_size;
    if(max_body_size.empty())
        throw std::runtime_error ("Error: no max_body_size specified" );
    if (location_brackets == true)
        current_location.client_max_body_size = max_body_size;
    else if (server_brackets == true)
        current_server.common.client_max_body_size = max_body_size;
}

void Config::handle_root(std::stringstream &ss)
{
    if (location_brackets == false && server_brackets == false)
    {
        throw std::runtime_error ("Error: root must be inside location or server brackets" );
        return ;
    }
    std::string root;
    ss >> root;
    if (root.empty())
        throw std::runtime_error ("Error: root must be followed by a path" );
    if (location_brackets == true)
        current_location.root = root;
    else if (server_brackets == true)
        current_server.common.root = root;
}

void Config::handle_location(std::stringstream &ss)
{
    if (server_brackets == false || location_brackets == true)
    {
        throw std::runtime_error ("Error: location must be inside server brackets" );
        return ;
    }
    current_location = LocationConfig();
    current_location.redir.first = -1;
    ss >> current_location.path;
    if (current_location.path.empty() || current_location.path[0] != '/')
        throw std::runtime_error ("Error: there is no location path " );

}

void Config::handle_methods(std::stringstream &ss)
{
    if (location_brackets == false && server_brackets == false)
    {
        throw std::runtime_error ("Error: methods must be inside location or server brackets" );
        return ;
    }
    std::string method;
    while (ss >> method)//in case we had multiple methods
    {
        if (location_brackets == true)
            current_location.methods.push_back(method);
        else if (server_brackets == true)
            current_server.common.methods.push_back(method);
    }
    if (location_brackets == true && current_location.methods.empty())
        throw std::runtime_error ("Error: there is no method specified" );
    else if (location_brackets == false && current_server.common.methods.empty())
        throw std::runtime_error ("Error: there is no method specified" );
}

void Config::handle_index(std::stringstream &ss)
{
    if(location_brackets == false && server_brackets == false)
    {
        throw std::runtime_error ("Error: index must be inside location or server brackets" );
        return ;
    }
    std::string index;
    ss >> index;
    if(index.empty())
        throw std::runtime_error ("Error: no index specified" );
    if (location_brackets == true)
        current_location.index = index;
    else if (server_brackets == true)
        current_server.common.index = index;

}

void Config::handle_autoindex(std::stringstream &ss)
{
    if (location_brackets == false && server_brackets == false)
    {
        throw std::runtime_error ("Error: autoindex must be inside location or server brackets" );
        return ;
    }
    std::string autoindex;
    ss >> autoindex;
    if (location_brackets == true)
    {
        if (autoindex == "on")
            current_location.autoindex = true;
        else if (autoindex == "off")
            current_location.autoindex = false;
        else
            throw std::runtime_error ("Error: autoindex must be on or off" );
    }
    else if (server_brackets == true)
    {
        if (autoindex == "on")
            current_server.common.autoindex = true;
        else if (autoindex == "off")
            current_server.common.autoindex = false;
        else
            throw std::runtime_error ("Error: autoindex must be on or off" );
    }
}

void Config::handle_return(std::stringstream &ss)
{
    if (location_brackets == false)
    {
        throw std::runtime_error ("Error: return must be inside location or server brackets" );
        return ;
    }
    else
    {
        int code;
        if (ss >> code)
            current_location.redir.first = code;
        else
            throw std::runtime_error ("Error: return must be followed by a code" );
        if (code < 200 || code > 599)
            throw std::runtime_error ("Error: http code should be  200 <= code <= 599");
        std::string url;
        ss >> url;
        current_location.redir.second = url;
    }
}

void Config::handle_upload(std::stringstream &ss)
{
    if (location_brackets == false)
    {
        throw std::runtime_error ("Error: upload must be inside location brackets" );
        return ;
    }
    else
    {
        std::string upload;
        ss >> upload;
        if (upload == "on")
            current_location.upload = true;
        else if (upload == "off")
            current_location.upload = false;
        else
            throw std::runtime_error ("Error: upload must be on or off" );
    }
}
void Config::handle_upload_path(std::stringstream &ss)
{
    if (location_brackets == false)
    {
        throw std::runtime_error ("Error: upload_path must be inside location brackets" );
        return ;
    }
    else
    {
        std::string upload_path;
        ss >> upload_path;
        if(upload_path.empty())
            throw std::runtime_error("Error: upload_path must be followed by a path");
        current_location.upload_path = upload_path;
    }
}

void Config::handle_cgi(std::stringstream &ss)
{
    if (location_brackets == false)
    {
        throw std::runtime_error ("Error: cgi must be inside location brackets" );
        return ;
    }
    else
    {
        std::string cgi;
        ss >> cgi;
        if (cgi == "on")
            current_location.cgi = true;
        else if (cgi == "off")
            current_location.cgi = false;
        else
            throw std::runtime_error ("Error: cgi must be on or off");
    }
}

void Config::handle_cgi_ext(std::stringstream &ss)
{
    if (location_brackets == false)
    {
        throw std::runtime_error ("Error: cgi_path must be inside location brackets");
        return ;
    }
    else
    {
        std::string ext;
        ss >> ext;
        std::string path;
        ss >> path;
        if(path.empty() || ext.empty())
            throw std::runtime_error("Error: cgi_path must be followed by an extension and a path");
        //current_location.cgi_ext.push_back(std::make_pair(ext, path));
        current_location.cgi_ext[ext] = path;
    }
}

const std::string & Config::get_key() const
{
    return key;
}

void Config::check_config()
{
    for (size_t i = 0; i < servers.size(); i++)
    {
        if (!servers[i].port.empty())//check the port is a number
        {
            for (size_t j = 0; j < servers[i].port.size(); j++)
                if (!isdigit(servers[i].port[j]))
                    throw std::runtime_error("Error: port must be a number");
        }
        else
            throw std::runtime_error("Error: no port specified");

        
        if (servers[i].common.client_max_body_size.empty())
            servers[i].common.client_max_body_size = "1000000"; // 1megabyte
        else
        {
            for (size_t j = 0; j < servers[i].common.client_max_body_size.size(); j++)
                if (!isdigit(servers[i].common.client_max_body_size[j]))
                    throw std::runtime_error("Error: client_max_body_size must be a number");
        }
        if (servers[i].common.root.empty())
            throw std::runtime_error("Error: no root specified");
        // if (servers[i].common.index.empty())
        //     throw std::runtime_error("Error: no index specified");
        // if (servers[i].common.autoindex == true && servers[i].common.index.empty())
        //     throw std::runtime_error("Error: autoindex is on but no index specified");
        // if (servers[i].common.methods.empty())
        //     throw std::runtime_error("Error: no method specified");
    }

}

void Config::printConfig()
{
    std::cout << "SERVER" << std::endl;
    for (size_t i = 0; i < servers.size();  i++)
    {
        std::cout << "server " << i + 1 << std::endl;
        std::cout << "port " << servers[i].port << std::endl;
        std::cout << "host " << servers[i].host << std::endl;
        std::cout << "server_name: " << servers[i].server_name << std::endl;
        std::cout << "error_page: " << std::endl;  
        
        
        std::cout << "COMM SERVER" << std::endl;
        for(std::map<std::string, std::string>::iterator it = servers[i].common.error_pages.begin(); it != servers[i].common.error_pages.end(); ++it)
            std::cout << "  " << it->first << " => " << it->second << '\n';
        std::cout << "client_max_body_size: " << servers[i].common.client_max_body_size << std::endl;
        std::cout << "root: " << servers[i].common.root << std::endl;
        std::cout << "methods: " << std::endl;
        
        for (size_t j = 0; j < servers[i].common.methods.size(); j++)
            std::cout << "  " << servers[i].common.methods[j] << std::endl;
        std::cout << "index: " << servers[i].common.index << std::endl;
        std::cout << "autoindex: " << servers[i].common.autoindex << std::endl;
        std::cout << "location: " << std::endl;
        
        std::cout << "LOCATIONS" << std::endl;
        for (size_t j = 0; j < servers[i].locations.size(); j++)
        {
            std::cout << "  " << servers[i].locations[j].path << std::endl;
            std::cout << "  error_page: " << std::endl;
            for(std::map<std::string, std::string>::iterator it = servers[i].locations[j].error_pages.begin(); it != servers[i].locations[j].error_pages.end(); ++it)
                std::cout << "    " << it->first << " => " << it->second << '\n';
            std::cout << "  client_max_body_size: " << servers[i].locations[j].client_max_body_size << std::endl;
            std::cout << "  root: " << servers[i].locations[j].root << std::endl;
            std::cout << "  methods: " << std::endl;
            
            
            
            std::cout << "LOC VEC " << std::endl;
            for (size_t k = 0; k < servers[i].locations[j].methods.size(); k++)
                std::cout << "    " << servers[i].locations[j].methods[k] << std::endl;
            std::cout << "  index: " << servers[i].locations[j].index << std::endl;
            std::cout << "  autoindex: " << servers[i].locations[j].autoindex << std::endl;
            std::cout << "  return: " << servers[i].locations[j].redir.first << " " << servers[i].locations[j].redir.second << std::endl;
            std::cout << "  upload: " << servers[i].locations[j].upload << std::endl;
            std::cout << "  upload_path: " << servers[i].locations[j].upload_path << std::endl;
            std::cout << "  cgi: " << servers[i].locations[j].cgi << std::endl;
            std::cout << "  cgi_ext: " << std::endl;
            // for (size_t k = 0; k < servers[i].locations[j].cgi_ext.size(); k++)
            //     std::cout << "    " << servers[i].locations[j].cgi_ext[k].first << " => " << servers[i].locations[j].cgi_ext[k].second << std::endl;
            
        }
    }
}
void Config::parseConfig()
{
    std::ifstream file(file_name.c_str());
    if (!file.is_open())
    {
        throw std::runtime_error ("Error: cannot open file" );
        return ;
    }
    std::map<std::string, void (Config::*)(std::stringstream &)> handlers;
    handlers["server"] = &Config::handle_server;
    handlers["{"] = &Config::handle_openBracket;
    handlers["}"] = &Config::handle_closedBracket;
    handlers["listen"] = &Config::handle_listen;
    handlers["host"] = &Config::handle_host;
    handlers["server_name"] = &Config::handle_server_name;
    handlers["error_page"] = &Config::handle_errorPages;
    handlers["client_max_body_size"] = &Config::handle_maxBodySize;
    handlers["root"] = &Config::handle_root;
    handlers["location"] = &Config::handle_location;
    handlers["allow"] = &Config::handle_methods;
    handlers["autoindex"] = &Config::handle_autoindex;
    handlers["index"] = &Config::handle_index;
    handlers["return"] = &Config::handle_return;
    handlers["upload"] = &Config::handle_upload;
    handlers["upload_path"] = &Config::handle_upload_path;
    handlers["cgi"] = &Config::handle_cgi;
    handlers["cgi_path"] = &Config::handle_cgi_ext;


    std::string line;
    while (std::getline(file, line))
    {
        std::stringstream ss(line);

        last_key = key;
        while (ss >> key)
        {
            if (handlers.find(key) != handlers.end())
            {
                (this->*handlers[key])(ss);//call the function for eexample  if the key is server its gonna calll handle_server function
            }
            else if (key[0] == '#')
            {
                //ignore the rest of the line
                ss.str("");//clear the string stream
                key = "";//clear the key
            }
            else
                throw std::runtime_error("Error: unknown key");
            last_key = key; 
        }
    }
    key = "";
    if (server_brackets == true || location_brackets == true)
        throw std::runtime_error("Error: server brackets not closed");
}
