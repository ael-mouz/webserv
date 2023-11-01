#pragma once

#include "Utils.hpp"
#include "../Request/Request_FSM.hpp"

class Client //can be inherit from Request
{
private:

public:
    int socketClient;
    // const ServerConf& serverConf;
    Request_Fsm request;
    bool read;

    bool write;
    // Client(ServerConf& serverConf, int socketClient);
    Client(int socketClient);
    Client& operator=(const Client& overl);
    Client(const Client& copy);
    ~Client();
};

std::string _Response(const Client &clients); // !!
