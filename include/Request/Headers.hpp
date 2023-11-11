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
	int requestChecker(Client &client);
    int ContentLenChecker(Client &client);
    int MultiPartChecker(Client &client, string& value);
    int MimeTypeChecker(Client &client, string& value);
	void reset();

    int deleteMthod(Client &client); // make it in response
	Headers();
	~Headers();
};
