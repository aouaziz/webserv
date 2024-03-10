#include "../includes/HTTP.hpp"

void    appendHeader(std::stringstream& header, const std::string& name, const std::string& value)
{
    if (!value.empty())
    {
        if (!name.empty())
            header << name << ": " << value << "\r\n";
        else
            header << value << "\r\n";

    }
}
void HTTP::SendResponseHeader(std::string CodeToSend, std::string Extention, std::string location, int Content_Length) // send Response header
{
    std::stringstream header;
    std::string date = this->_linker.getDate();
    appendHeader(header, "", "HTTP/1.1 " + CodeToSend + " " + this->_linker.Status_codes_error[CodeToSend]);
    appendHeader(header, "Server", this->_config.server_name);
    appendHeader(header, "Date", date);
    appendHeader(header, "Location", location);
    appendHeader(header, "Content-Type", Extention);
    if (Content_Length)
        header << "Content-Length: " << Content_Length << "\r\n";
    header << "Connection: keep-alive\r\n";
    header << "\r\n";
    Response = header.str();
    this->Response.append(this->Request_header["body"]);
    response_ready = true;
}

std::string     readFileIntoString(const std::string& filename)
{
    std::ifstream file(filename.c_str());
    std::cout << filename << std::endl;
    if (file)
    {
        std::stringstream req_data;
        req_data << file.rdbuf();
        file.close();
        return req_data.str();
    }
    return "";
}


std::string         generateErrorPage(std::string CodeToSend, std::string statusMessage, std::string filename)
{
    std::string buffer;
    std::string res;
    std::ifstream file(filename.c_str());
    if (file)
    {
        std::stringstream req_data;
        req_data << file.rdbuf();
        file.close();
        res = req_data.str();
        res.insert(res.find("<title><") + 7, CodeToSend + " - " + statusMessage);
        res.insert(res.find("<h1><") + 4, CodeToSend + " - " + statusMessage);
    }
    else
        res = "<!DOCTYPE html><html><head><title>" + CodeToSend + " " + statusMessage + "</title><style>body {font-family: Arial, sans-serif;display: flex;justify-content: center;align-items: center;height: 100vh;margin: 0;text-align: center;}.content {transform: translateY(-50%);}.path {margin-top: 20px;font-style: italic;}.back-link {margin-top: 20px;color: #007BFF;text-decoration: none;}.back-link:hover {color: #0056b3;text-decoration: underline;}</style></head><body><div class='content'><h1><span class='span'>" + CodeToSend + "</span> " + statusMessage + "</h1><style>.path {background-color: rgba(130, 130, 130, 0.045);padding: 0.2rem 1rem;border: solid;border-color: rgba(67, 67, 67, 0.066);border-radius: 0.40rem;background-color: rgba(255, 0, 0, 0.011) ;box-shadow: transparent 0 10 0.2rem 0.1rem;}.span {color: red;}</style></div></body></html>";
    return res;
}


void HTTP::   sendCodeResponse(std::string CodeToSend) 
{
    std::string Content;
    std::string filename;
    if (this->_config.common.error_pages.find(CodeToSend) != this->_config.common.error_pages.end())
    {
        filename = this->_config.common.error_pages[CodeToSend];
        Content = readFileIntoString(filename);
    }
    if (Content.empty())
    {
        std::string statusMessage = this->_linker.Status_codes_error[CodeToSend];
        Content = generateErrorPage(CodeToSend, statusMessage, filename);
    }
    this->Request_header["body"] = Content;
    this->Request_header["Content-Type"] = "text/html";
    this->Request_header["Content-Length"] = to_string(Content.length());
    SendResponseHeader(CodeToSend, ".html", "", Content.length());
}