#pragma once

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

		std::string RandomFileName(const std::string& path, const std::string& fileExtension);
		std::string FileExtention(std::string fileName);
		std::string redirectionPage(const std::string& newLocation);
		std::string getDate();
};