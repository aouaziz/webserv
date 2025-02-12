#ifndef LINKER_HPP
#define LINKER_HPP

#include <iostream>
#include <map>
#include <cstring>
#include <sstream>
#include <vector>
#include <fstream>
#include "include.hpp"

class Linker
{
	public:
		Linker();
		MapOf_Str_Str		File_extensions;
    	MapOf_Str_Str		Status_codes_error;
    	MapOf_Str_Str		Mime_types;

		std::string RandomFileName(const std::string& path, const std::string& fileExtension);
		std::string FileExtention(std::string fileName);
};

#endif