#pragma once
#include <string>

std::string decode_body(std::string const &encoded_body);
std::string encode_body(std::string const &body);