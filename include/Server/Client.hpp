#pragma once

#include "../../include/Config/ServerConf.hpp"
#include "../Request/Request.hpp"
#include "../Response/Response.hpp"
#include "Utils.hpp"

class Client
{
public:
	int socketClient;
	bool readEvent;
	bool writeEvent;
	ServerConf &serverConf;
	std::string clientHost;
	std::string clientIP;
	Response response;
	Request request;
	Client(ServerConf &serverConf, int socketClient, std::string clientIP, std::string host);
	Client &operator=(const Client &overl);
	Client(const Client &copy);
	~Client();
};
