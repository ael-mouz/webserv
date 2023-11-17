#pragma once

#include "../Server/Utils.hpp"

class MimeTypes
{
private:
	std::map<std::string, std::string> mimeTypes_;

public:
	std::string getMimeType(const std::string &extension) const;
	std::string getExtensionMimeType(const std::string &extension) const;

public:
	MimeTypes();
	~MimeTypes();
};
