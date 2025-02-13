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
	string key;
	string hold;
	bool isContentTypeInHeaders;
	bool isContentLengthInHeaders;
	int statu(Client &client, const string &errorMsg, int statu);
	int requestChecker(Client &client);
	int contentLenChecker(Client &client);
	int contentTypeChecker(Client &client);
	int multiPartChecker(Client &client, string &value);
	int mimeTypeChecker(Client &client, string &value);
	int cgiChecker(Client &client);
	int transEncodChecker(Client &client);

public:
	int read(Client &client, string &buffer, ssize_t &size);
	void reset();
	Headers();
	~Headers();
};
