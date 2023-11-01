#pragma once

#include "../../include/Config/Config.hpp"
#include "../../include/Config/ServerConf.hpp"
#include "../../include/Server/Utils.hpp"
#include "../../include/Server/Server.hpp"
#include "../../include/Server/Client.hpp"
// #include "../include/Config/Config.hpp"

class RunServers
{
public:
    vector<Server> servers;
    vector<Client> clients;
    fd_set  readFds;
    fd_set  writeFds;
    fd_set tmp; ////
    int maxFds;
    int newSocket;
    int numberOfEvents;
    void init(vector<ServerConf>& ServerConf);
    void runing(vector<Server>& servers, int maxfd);
    void runingServers();
};