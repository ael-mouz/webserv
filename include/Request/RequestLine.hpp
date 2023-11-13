#pragma once

#include "../Server/Utils.hpp"

class Request;

enum requestState
{
	METHOD,
	_URI,
	URI_SPACE,
	VERSION,
    DECODE_URI,
};

class RequestLine
{
private:
	int count;
    int count_hexa;
public:
	int read(Request &request, string &buffer, ssize_t &size);
	void reset();
	RequestLine();
	~RequestLine();
};
