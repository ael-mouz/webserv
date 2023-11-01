#pragma once
#include "../Config/ServerConf.hpp"
// #include "Client.hpp"

class Server
{
public:
	ServerConf serverConf;
	int socketServer;
	int init();
};

// void RunServers(vector<Server>& servers, int maxfd);
