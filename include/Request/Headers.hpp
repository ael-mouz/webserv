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
	void read(Client &client, string &buffer, ssize_t &size);
	void checkrequest(Client &client);
	void reset();
	Headers();
	~Headers();
};
