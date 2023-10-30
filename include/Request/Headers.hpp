#pragma once

#include "utils.hpp"

class Request_Fsm;

enum HeaderState
{
    CHECK,
    KEY,
    VALUE,
    END_VALUE,
    END_HEADERS,
};

class Headers
{
    int count;
public:
    void read (Request_Fsm &Request, string& buffer, ssize_t& size);
    void checkrequest (Request_Fsm &Request);
    Headers();
    ~Headers();
};


