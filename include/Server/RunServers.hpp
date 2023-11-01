#pragma once

#include "../../include/Config/Config.hpp"
#include "../../include/Config/ServerConf.hpp"
#include "../../include/Server/Utils.hpp"
#include "../../include/Server/Client.hpp"

struct Server
{
	ServerConf serverConf;
	int socketServer;
};

class RunServers
{
    fd_set  readFds;
    fd_set  writeFds;
    fd_set serverFds; ////
    int maxFds;
    int newSocket;
    int numberOfEvents;
public:
    vector<Server> servers;
    vector<Client> clients;
    int bindSockets(Server& server);
    void acceptClients();
    void runing();
    void resetFds();
    RunServers(vector<ServerConf>& serverConf);
    ~RunServers();
};