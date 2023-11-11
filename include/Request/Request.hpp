#pragma once

#include "../Server/Utils.hpp"
#include "ChunkedEncoding.hpp"
#include "Headers.hpp"
#include "Body.hpp"
#include "RequestLine.hpp"

class Client;

enum _mainState
{
	REQUEST_LINE,
	HEADERS,
	// BODY,
    MultiPart,
    CGI,
    MIMETYPES,
};

struct File
{
	string fileName;
	multimap<string, string> Content;
    // bool toDelete;
};

class Request
{
private:
	RequestLine requestLine;
	Headers headers;
	ChunkedEncoding decode;
	Body body;

public:
	int mainState;
	int subState;
	string hold;
	string key;
	string boundary; // put it is header classe and make a geter
	int sizeBoundary; // put it is header classe and make a geter
    bool isCGI;
	bool decodeFlag;
	size_t ContentLength;
	int ReqstDone;
	string Method;
	string URI;
	multimap<string, string> mapHeaders;
	vector<File> files;
    bool deleteFiles;
	void read(Client &client, string &buffer, ssize_t &size);
	void reset(void);
    bool openBodyFile(const string &path, const string &extension);
	Request();
	~Request();
};
