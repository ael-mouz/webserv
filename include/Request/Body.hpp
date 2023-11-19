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
    string boundary;
	int sizeBoundary;

public:
	int multiPart(Client &client, string &buffer, ssize_t &size);
	int writeBody(Client &client, string &buffer, ssize_t &size);
    int skipBody(Client &client, ssize_t &size);
	int createFile(Client &client,const string &value, std::string &fileName);
	bool RandomFile(Request &Request, const string& path, const string& extension);
    int statu(Client &client, const string& errorMsg, int statu);
    bool isEncodChunk(Client &client);
    void setBoundary(const string& boundary);
    string getBoundary(void) const;
    void setSizeBoundary(const int& sizeBoundary);
    int getSizeBoundary(void) const;
	void reset();
	Body();
	~Body();
};
