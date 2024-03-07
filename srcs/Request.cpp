#include "../includes/Request.hpp"


Request::Request(ServerConfig   config):config(config){
    BodyLength = 0;
}

Request::~Request()
{
}

// bool Request::parse_request(std::string request_header) {
//     // Split the request into lines
//     std::vector<std::string> lines;
//     std::istringstream iss(request_header);
//     std::string line;
//     while (std::getline(iss, line)) {
//         replaceCarriageReturn(line);
//         lines.push_back(line);
//     }

//     // Parse the first line (request line)
//     std::istringstream first_line_iss(lines[0]);
//     first_line_iss >> Method >> Req_Uri >> version;

//     // Parse the remaining lines (headers)
//     for (size_t i = 1; i < lines.size(); ++i) {
//         std::string header_line = lines[i];
//         std::size_t colon_pos = header_line.find(':');
//         if (colon_pos != std::string::npos) {
//             std::string key = header_line.substr(0, colon_pos);
//             std::string value = header_line.substr(colon_pos + 2); // +2 to skip ':' and space
//             headers[key] = value;
//             if(key == "Content-Length")
//             {
//                 try
//                 {
//                     BodyLength = std::atoi(value.c_str());
//                 }
//                 catch(const std::exception& e){//error
//                 }
//             }
//             else
//                 return true;
//         }
//     }
//     return false ; 
// }