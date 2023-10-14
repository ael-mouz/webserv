#include "../include/Config.hpp"

std::string trim(const std::string &str, const std::string &charactersToTrim)
{
	if (str.empty())
		return str;
	size_t start = str.find_first_not_of(charactersToTrim);
	if (start == std::string::npos)
		return "";
	size_t end = str.find_last_not_of(charactersToTrim);
	return str.substr(start, end - start + 1);
}
