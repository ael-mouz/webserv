#pragma once

#include "../../include/Config/ServerConf.hpp"
#include "../Request/Request_FSM.hpp"
#include "../Response/Response.hpp"
#include "Utils.hpp"

class Client
{

public:
	std::string clientIP;
	int socketClient;
	ServerConf &serverConf;
	Response response;
	Request_Fsm request;
	bool read;
	bool write;
	Client(ServerConf &serverConf, int socketClient, std::string clientIP);
	Client &operator=(const Client &overl);
	Client(const Client &copy);
	~Client();
};
