#pragma once

#include "utils.hpp"

class Request_Fsm;

enum methods
{
    GET = 3,
    POST = 4,
    DELETE = 6,
};

enum requestState
{
    REQUEST,
    METHOD,
    METHOD_SPACE,
    _URI,
    URI_SPACE,
    VERSION,
    CR,
    LF,
};

class RequestLine
{
    int count;
    int method;
    map <int, string> PossiblMethod; //global ??
public:
    void read(Request_Fsm &Request, string& buffer, ssize_t& size);
    RequestLine();
    ~RequestLine();
};
