#pragma once

#include "../Server/Utils.hpp"

class Request;
class Client;

enum multiPartState
{
	FIRST_BOUNDARY,
	AFTER_BOUNDARY,
	LF_BOUNDARY,
	HANDLER_KEY,
	HANDLER_VALUE,
	LF_VALUE,
	CHECK_AFTER_VALUE,
	START_DATA,
	WRITE_DATA,
	CHECK_END,
	CR_END,
	LF_END,
};

class Body
{
private:
	int count;
	string key;
    string hold;
	FILE *fileF;
	size_t countLength;

public:
	void multiPart(Client &client, string &buffer, ssize_t &size);
	void writeBody(Client &client, string &buffer, ssize_t &size);
	int createFile(Client &client,const string &value, std::string &fileName);
	bool RandomFile(Request &Request, const string& path, const string& extension);
    bool isEncodChunk(Client &client);
	void reset();
	Body();
	~Body();
};
