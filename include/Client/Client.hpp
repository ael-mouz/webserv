#pragma once
#include "../Utils.hpp"
#include "../Request/Request_FSM.hpp"

class Client //can be inherit from Request
{
private:

public:
    int fd;
    Request_Fsm request;
    bool read;
    bool write;
    Client(int fd);
    Client& operator=(const Client& overl);
    Client(const Client& copy);
    ~Client();
};

std::string Response(const Client& clients); // !!
