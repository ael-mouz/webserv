#pragma once
#include "../Server/Utils.hpp"
#include "ChunkedEncoding.hpp"
#include "RequestLine.hpp"
#include "Headers.hpp"
#include "Multipart.hpp"
#include "../Server/Server.hpp"

enum _mainState
{
	ERROR,
	REQUEST_LINE,
	HEADERS,
	BODY,
	END,
};

struct File
{
	string fileName;
	multimap<string, string> Content;
};

class Request_Fsm // : public Base //can be inherited from server
{
private:
	RequestLine requestLine;
	Headers headers;
	Multipart multi; // creat body and handel all cases in body
	ChunkedEncoding decode;

public:
    const ServerConf& serverConf;
	int mainState;
	int subState;
	string hold;
    string key;
	string boundary; // change to char*
	bool decodeFlag;
	int sizeBoundary;
	size_t ContentLength;
	int ReqstDone;
	string Method; // lot of memory
	string URI;
	multimap<string, string> mapHeaders;
	vector<File> files;
	void read(string &buffer, ssize_t &size);
	void clear(void);
	Request_Fsm(const ServerConf& serverConf);
    Request_Fsm &operator=(const Request_Fsm& overl);
    Request_Fsm(const Request_Fsm& copy);
	~Request_Fsm();
};
