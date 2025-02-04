#include "../includes/HTTPMethod.hpp"


void HTTPMethod::composeResponseHeader(std::string statusCode, std::string contentType, std::string location, int contentLength)
{
    std::stringstream responseHeader;
    
    responseHeader << "HTTP/1.1 " << statusCode << " " << _linker.Status_codes_error[statusCode] << "\r\n";
    responseHeader << "Server: " << _config.server_name << "\r\n";

    if (!location.empty())
        responseHeader << "Location: " << location << "\r\n";
    if (contentLength > 0) 
        responseHeader << "Content-Length: " << contentLength << "\r\n";

    responseHeader << "Content-Type: " << contentType << "\r\n";
    responseHeader << "Connection: keep-alive\r\n";
    responseHeader << "\r\n";
    Response = responseHeader.str() + Request_header["body"];
    response_ready = true;
}


std::string generateDefaultErrorPage(std::string statusCode, std::string statusMessage) {
    return "<!DOCTYPE html><html><head><title>" + statusCode + " UwU " + statusMessage + "</title><style>body {font-family: 'Arial', sans-serif;background-color: #fce9ff;}.container {display: flex;justify-content: center;align-items: center;height: 100vh;}.content {text-align: center;padding: 50px;background-color: #fff;border-radius: 12px;box-shadow: 0px 6px 20px rgba(0, 0, 0, 0.15);}.error-code {font-size: 48px;font-weight: bold;color: #ff69b4;}.error-message {font-size: 24px;color: #ff69b4;margin-top: 20px;}.back-link {margin-top: 20px;color: #800080;text-decoration: none;font-size: 18px;}.back-link:hover {color: #4b0082;text-decoration: underline;}</style></head><body><div class='container'><div class='content'><div class='error-code'>" + statusCode + "</div><div class='error-message'>" + statusMessage + "</div><a href='/' class='back-link'>Go back to Home</a></div></div></body></html>";
}


void HTTPMethod::   sendCodeResponse(std::string statusCode) 
{
    std::string errorPageContent;
    if (this->_config.common.error_pages.find(statusCode) != this->_config.common.error_pages.end())// Check if there's a configured error page for the given status code
    {
        // std::cerr << "Error page found for status code " << statusCode << std::endl;
        std::ifstream errorPageFile(this->_config.common.error_pages[statusCode].c_str());
        // std::cerr << "Error page file: " << this->_config.common.error_pages[statusCode] << std::endl;
        if(errorPageFile)
        {
            std::stringstream buffer;
            buffer << errorPageFile.rdbuf();
            errorPageContent = buffer.str();
            errorPageFile.close();
        }
    }
    if(errorPageContent.empty())    // If there's no configured error page, generate a default one
    {
        std::string statusMessage = this->_linker.Status_codes_error[statusCode];
        errorPageContent = generateDefaultErrorPage(statusCode, this->_linker.Status_codes_error[statusCode]);
    }

    this->Request_header["body"] = errorPageContent;
    this->Request_header["Content-Type"] = "text/html";
    this->Request_header["Content-Length"] = to_string(errorPageContent.length());
    composeResponseHeader(statusCode, ".html", "", errorPageContent.length());
}