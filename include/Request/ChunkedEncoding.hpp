#pragma once

#include "utils.hpp"
#include <string>

class Request_Fsm;

enum chunkedEncoding{
    HEXA,
    LF_AFTER_HEXA,
    SKIP_BODY,
    CR_BEFOR_HEXA,
    LF_BEFOR_HEXA,
};

class ChunkedEncoding
{
    size_t count;
    int decodeState;
    string hold;
    size_t countLength;
public:
    void read(Request_Fsm &Request, string& buffer, ssize_t& size);
    ChunkedEncoding();
    ~ChunkedEncoding();
};