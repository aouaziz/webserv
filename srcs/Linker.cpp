#include "../includes/include.hpp"
#include "../includes/Linker.hpp"
#include "../includes/Config.hpp"
#include "../includes/HTTP.hpp"
#include "../includes/Server.hpp"

Linker::Linker()
{
	this->File_extensions["text/html"]= ".html";
	this->File_extensions["text/css"]= ".css";
	this->File_extensions["application/javascript"]= ".js";
	this->File_extensions["application/json"]= ".json";
	this->File_extensions["application/xml"]= ".xml";
	this->File_extensions["text/plain"]= ".txt";
	this->File_extensions["image/jpeg"]= ".jpg";
	this->File_extensions["image/png"]= ".png";
	this->File_extensions["image/gif"]= ".gif";
	this->File_extensions["image/bmp"]= ".bmp";
	this->File_extensions["image/x-icon"]= ".ico";
	this->File_extensions["application/pdf"]= ".pdf";
	this->File_extensions["application/zip"]= ".zip";
	this->File_extensions["application/x-tar"]= ".tar";
	this->File_extensions["application/gzip"]= ".gz";
	this->File_extensions["application/x-rar-compressed"]= ".rar";
	this->File_extensions["application/x-7z-compressed"]= ".7z";
	this->File_extensions["audio/mpeg"]= ".mp3";
	this->File_extensions["audio/wav"]= ".wav";
	this->File_extensions["video/mp4"]= ".mp4";
	this->File_extensions["video/x-msvideo"] = ".avi";
	this->File_extensions["application/vnd.ms-powerpoint"] = ".ppt";
	this->File_extensions["application/vnd.openxmlformats-officedocument.presentationml.presentation"] = ".pptx";
	this->File_extensions["application/msword"] = ".doc";
	this->File_extensions["application/vnd.openxmlformats-officedocument.wordprocessingml.document"] = ".docx";
	this->File_extensions["application/vnd.ms-excel"] = ".xls";
	this->File_extensions["application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"] = ".xlsx";
	this->File_extensions["text/csv"] = ".csv";
	this->File_extensions["application/x-shockwave-flash"] = ".swf";
	this->File_extensions["image/svg+xml"] = ".svg";
	this->File_extensions["video/mpeg"] = ".mpg";
	this->File_extensions["video/webm"] = ".webm";
	this->File_extensions["audio/ogg"] = ".ogg";
	this->File_extensions["video/ogg"] = ".ogg";
	this->File_extensions["image/webp"] = ".webp";
	this->File_extensions["image/tiff"] = ".tif";
	this->File_extensions["application/font-woff"] = ".woff";
	this->File_extensions["application/font-woff2"] = ".woff2";
	this->File_extensions["application/x-font-ttf"] = ".ttf";
	this->File_extensions["application/x-font-opentype"] = ".otf";
	this->File_extensions["application/vnd.ms-fontobject"] = ".eot";
	this->File_extensions["application/octet-stream"] = ".bin";
	this->File_extensions["application/x-perl"] = ".pl";
	this->File_extensions["application/x-httpd-php"] = ".php";
	this->File_extensions["video/quicktime"] = ".mov";

	// MIME Types
	this->Mime_types[".html"]	= "text/html";
	this->Mime_types[".htm"]	= "text/html";
	this->Mime_types[".css"]	= "text/css";
	this->Mime_types[".js"]		= "application/javascript";
	this->Mime_types[".json"]	= "application/json";
	this->Mime_types[".xml"]	= "application/xml";
	this->Mime_types[".txt"]	= "text/plain";
	this->Mime_types[".jpg"]	= "image/jpeg";
	this->Mime_types[".jpeg"]	= "image/jpeg";
	this->Mime_types[".png"]	= "image/png";
	this->Mime_types[".gif"]	= "image/gif";
	this->Mime_types[".bmp"]	= "image/bmp";
	this->Mime_types[".ico"]	= "image/x-icon";
	this->Mime_types[".pdf"]	= "application/pdf";
	this->Mime_types[".zip"]	= "application/zip";
	this->Mime_types[".tar"]	= "application/x-tar";
	this->Mime_types[".gz"]		= "application/gzip";
	this->Mime_types[".rar"]	= "application/x-rar-compressed";
	this->Mime_types[".7z"]		= "application/x-7z-compressed";
	this->Mime_types[".mp3"]	= "audio/mpeg";
	this->Mime_types[".wav"]	= "audio/wav";
	this->Mime_types[".mp4"]	= "video/mp4";
	this->Mime_types[".avi"]	= "video/x-msvideo";
	this->Mime_types[".ppt"]	= "application/vnd.ms-powerpoint";
	this->Mime_types[".pptx"]	= "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	this->Mime_types[".doc"]	= "application/msword";
	this->Mime_types[".docx"]	= "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	this->Mime_types[".xls"]	= "application/vnd.ms-excel";
	this->Mime_types[".xlsx"]	= "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	this->Mime_types[".csv"]	= "text/csv";
	this->Mime_types[".swf"]	= "application/x-shockwave-flash";
	this->Mime_types[".svg"]	= "image/svg+xml";
	this->Mime_types[".mpg"]	= "video/mpeg";
	this->Mime_types[".mpeg"]	= "video/mpeg";
	this->Mime_types[".webm"]	= "video/webm";
	this->Mime_types[".ogg"]	= "audio/ogg";
	this->Mime_types[".ogg"]	= "video/ogg";
	this->Mime_types[".webp"]	= "image/webp";
	this->Mime_types[".tif"]	= "image/tiff";
	this->Mime_types[".tiff"]	= "image/tiff";
	this->Mime_types[".woff"]	= "application/font-woff";
	this->Mime_types[".woff2"]	= "application/font-woff2";
	this->Mime_types[".ttf"]	= "application/x-font-ttf";
	this->Mime_types[".otf"]	= "application/x-font-opentype";
	this->Mime_types[".eot"]	= "application/vnd.ms-fontobject";
	this->Mime_types[".bin"]	= "application/octet-stream";
	this->Mime_types[".php"]	= "application/x-httpd-php";
	this->Mime_types[".pl"]		= "application/x-perl";
	this->Mime_types[".mov"]	= "video/quicktime";

	// Status codes
	this->Status_codes_error["100"] = "Continue";
	this->Status_codes_error["101"] = "Switching Protocols";
	this->Status_codes_error["102"] = "Processing";
	this->Status_codes_error["200"] = "OK";
	this->Status_codes_error["201"] = "Created";
	this->Status_codes_error["202"] = "Accepted";
	this->Status_codes_error["203"] = "Non-Authoritative Information";
	this->Status_codes_error["204"] = "No Content";
	this->Status_codes_error["205"] = "Reset Content";
	this->Status_codes_error["206"] = "Partial Content";
	this->Status_codes_error["207"] = "Multi-Status";
	this->Status_codes_error["208"] = "Already Reported";
	this->Status_codes_error["226"] = "IM Used";
	this->Status_codes_error["300"] = "Multiple Choices";
	this->Status_codes_error["301"] = "Moved Permanently";
	this->Status_codes_error["302"] = "Found";
	this->Status_codes_error["303"] = "See Other";
	this->Status_codes_error["304"] = "Not Modified";
	this->Status_codes_error["305"] = "Use Proxy";
	this->Status_codes_error["307"] = "Temporary Redirect";
	this->Status_codes_error["308"] = "Permanent Redirect";
	this->Status_codes_error["400"] = "Bad Request";
	this->Status_codes_error["401"] = "Unauthorized";
	this->Status_codes_error["402"] = "Payment Required";
	this->Status_codes_error["403"] = "Forbidden";
	this->Status_codes_error["404"] = "Not Found";
	this->Status_codes_error["405"] = "Method Not Allowed";
	this->Status_codes_error["406"] = "Not Acceptable";
	this->Status_codes_error["407"] = "Proxy Authentication Required";
	this->Status_codes_error["408"] = "Request Timeout";
	this->Status_codes_error["409"] = "Conflict";
	this->Status_codes_error["410"] = "Gone";
	this->Status_codes_error["411"] = "Length Required";
	this->Status_codes_error["412"] = "Precondition Failed";
	this->Status_codes_error["413"] = "Payload Too Large";
	this->Status_codes_error["414"] = "Url Too Long";
	this->Status_codes_error["415"] = "Unsupported Media Type";
	this->Status_codes_error["416"] = "Range Not Satisfiable";
	this->Status_codes_error["417"] = "Expectation Failed";
	this->Status_codes_error["418"] = "I'm a teapot";
	this->Status_codes_error["421"] = "Misdirected Request";
	this->Status_codes_error["422"] = "Unprocessable Entity";
	this->Status_codes_error["423"] = "Locked";
	this->Status_codes_error["424"] = "Failed Dependency";
	this->Status_codes_error["425"] = "Too Early";
	this->Status_codes_error["426"] = "Upgrade Required";
	this->Status_codes_error["428"] = "Precondition Required";
	this->Status_codes_error["429"] = "Too Many Requests";
	this->Status_codes_error["431"] = "Request Header Fields Too Large";
	this->Status_codes_error["451"] = "Unavailable For Legal Reasons";
	this->Status_codes_error["500"] = "Internal Server Error";
	this->Status_codes_error["501"] = "Not Implemented";
	this->Status_codes_error["502"] = "Bad Gateway";
	this->Status_codes_error["503"] = "Service Unavailable";
	this->Status_codes_error["504"] = "Gateway Timeout";
	this->Status_codes_error["505"] = "HTTP Version Not Supported";
	this->Status_codes_error["506"] = "Variant Also Negotiates";
	this->Status_codes_error["507"] = "Insufficient Storage";
	this->Status_codes_error["508"] = "Loop Detected";
	this->Status_codes_error["510"] = "Not Extended";
	this->Status_codes_error["511"] = "Network Authentication Required";
}


std::string Linker::RandomFileName(const std::string& path, const std::string& fileExtension)
{
	std::string fileName = path;
    const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	std::srand(std::time(NULL));
    for (int i = 0; i < 10; ++i)
        fileName += characters[std::rand() % characters.length()];
    fileName += fileExtension;
    return fileName;
}

std::string Linker::FileExtention(std::string fileName) {
	int	DotPlacement = fileName.find_last_of('.');
    int lenght = fileName.length();

    if (lenght == 0 || DotPlacement < 0)
        return ("");
    if (DotPlacement > 0) {
	    std::string ex = fileName.substr(DotPlacement ,lenght);
		return (ex);
	}
	return ("");
}

// Get Date for header

std::string Linker::getDate()
{
	std::string date;
	time_t now = time(0);
	struct tm tstruct;
	char buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%a, %d %b %Y %X GMT", &tstruct);
	date = buf;
	return (date);
}


std::string Linker::redirectionPage(const std::string& newLocation) {
    std::stringstream html;
    html << "<!DOCTYPE html>\n"
         << "<html>\n"
         << "<head>\n"
         << "    <meta http-equiv=\"refresh\" content=\"0;url=" << newLocation << "\" />\n"
         << "</head>\n"
         << "<body>\n"
         << "    <p>The page has moved to <a href=\"" << newLocation << "\">" << newLocation << "</a>.</p>\n"
         << "</body>\n"
         << "</html>";
    return html.str();
}