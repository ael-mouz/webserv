#pragma once
#include "./Config/ServerConf.hpp"
#include "./Client/Client.hpp"

class Server
{
public:
	ServerConf serverConf;
	int socketServer;
	int maxfd;
	std::vector<Client> Clients;
	void init();
};
