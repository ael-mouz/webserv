#pragma once

#include "ServerConfig.hpp"

class ServerConf
{
public:
	ServerConfig DefaultServerConfig;
	std::map<std::string, ServerConfig> OtherServerConfig;
};
