#pragma once

#include "../Server/Utils.hpp"

class Client;

enum requestState
{
	METHOD,
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

public:
	int read(Client &client, string &buffer, ssize_t &size);
    // int checker(Client &client);
	void reset();
	RequestLine();
	~RequestLine();
};
