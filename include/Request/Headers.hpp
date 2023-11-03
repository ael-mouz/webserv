#pragma once

#include "../Server/Utils.hpp"

class Client;

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
private:
	int count;

public:
	int read(Client &client, string &buffer, ssize_t &size);
	int checkrequest(Client &client);
	void reset();
	Headers();
	~Headers();
};
