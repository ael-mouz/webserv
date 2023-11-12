#pragma once

#include "../../include/Config/ServerConf.hpp"
#include "../Request/Request.hpp"
#include "../Response/Response.hpp"
#include "Utils.hpp"

class Client
{

public:
	int socketClient;
	std::string clientHost;
	std::string clientIP;
	ServerConf &serverConf;
	Response response;
	Request request;
	bool read;
	bool write;
	Client(ServerConf &serverConf, int socketClient, std::string clientIP,std::string host);
	Client &operator=(const Client &overl);
	Client(const Client &copy);
	~Client();
};
