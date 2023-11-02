#pragma once

#include "../../include/Config/ServerConf.hpp"
#include "../Request/Request_FSM.hpp"
#include "../Response/Response.hpp"
#include "Utils.hpp"

class Client
{

public:
	int socketClient;
	ServerConf &serverConf;
	Response response;
	Request_Fsm request;
	bool read;
	bool write;
	Client(ServerConf &serverConf, int socketClient);
	Client &operator=(const Client &overl);
	Client(const Client &copy);
	~Client();
};

std::string _Response(const Client &clients); // !!
