#pragma once

#include "../Server/Utils.hpp"

class Status
{
private:
	std::map<std::string, std::string> status;

public:
	Status();
	~Status();
	std::string getStatus(const std::string &status) const;
};
