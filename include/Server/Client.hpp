#pragma once

#include "../../include/Config/ServerConf.hpp"
#include "../Request/Request_FSM.hpp"
#include "Utils.hpp"

class Client {

  public:
    int socketClient;
    const ServerConf &serverConf;
    Request_Fsm request;
    bool read;
    bool write;
    Client(ServerConf &serverConf, int socketClient);
    Client &operator=(const Client &overl);
    Client(const Client &copy);
    ~Client();
};

std::string _Response(const Client &clients); // !!
