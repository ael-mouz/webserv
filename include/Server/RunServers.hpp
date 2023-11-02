#pragma once

#include "../../include/Config/Config.hpp"
#include "../../include/Config/ServerConf.hpp"
#include "../../include/Server/Client.hpp"
#include "../../include/Server/Utils.hpp"

struct Server {
    ServerConf serverConf;
    int socketServer;
};

class RunServers {
    fd_set readFds;
    fd_set writeFds;
    fd_set serverFds;
    int maxFds;
    int newSocket;
    int numberOfEvents;
    char *recvbuffer;
  public:
    vector<Server> servers;
    vector<Client> clients;
    int bindSockets(Server &server);
    void resetFds();
    void acceptClients();
    void receiveData(vector<Client>::iterator &it, ssize_t &size);
    void runing();
    RunServers(char **av);
    ~RunServers();
};