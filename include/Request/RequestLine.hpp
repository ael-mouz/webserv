#pragma once

#include "../Server/Utils.hpp"

class Client;

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
private:
	int count;
	int method;
	map<int, string> PossiblMethod; // global ??

public:
	int read(Client &client, string &buffer, ssize_t &size);
    // void checker(Client &client);
	void reset();
	RequestLine();
	~RequestLine();
};
