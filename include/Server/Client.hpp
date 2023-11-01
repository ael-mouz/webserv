#pragma once
#include "Utils.hpp"
#include "../Request/Request_FSM.hpp"
#include "../Response/Response.hpp"

class Client //can be inherit from Request
{
private:

public:
    int socketClient;
    ServerConf& serverConf;
    Request_Fsm request;
	Response response;
	bool read;
	bool write;
    Client(ServerConf& serverConf, int socketClient);
    Client& operator=(const Client& overl);
    Client(const Client& copy);
    ~Client();
};

std::string _Response( Client& clients); // !!
