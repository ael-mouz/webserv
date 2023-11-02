#pragma once

#include "../Server/Utils.hpp"

class MimeTypes
{
private:
	std::map<std::string, std::string> mimeTypes_;

public:
	MimeTypes();
	~MimeTypes();
	std::string getMimeType(const std::string &extension) const;
};
