#pragma once

#include "../../include/Config/Config.hpp"
#include "../../include/Config/ServerConf.hpp"
#include "../../include/Server/Client.hpp"
#include "../../include/Server/Utils.hpp"

#define CLIENT_BODY_TIMEOUT 60000

struct Server
{
	ServerConf serverConf;
	int socketServer;
};

class RunServers
{
private:
	fd_set readFds;
	fd_set writeFds;
	fd_set serverFds;
	int maxFds;
	int maxFdsServers;
	int newSocket;
	int numberOfEvents;
	struct timeval timeout;
	char *recvbuffer;
	vector<Server> servers;
	vector<Client> clients;

private:
	int bindSockets(Server &server);
	void resetFds();
	void hardReset();
	void acceptClients();
	bool receiveData(vector<Client>::iterator &it);
	bool sendData(vector<Client>::iterator &it);
	void timeoutChecker();
	void timeoutClientChecker(Client &client);

public:
	void runing();
	RunServers(char **av);
	~RunServers();
};
