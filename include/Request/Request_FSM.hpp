#pragma once
#include "../Request/ChunkedEncoding.hpp"
#include "../Utils.hpp"
#include "../Request/RequestLine.hpp"
#include "../Request/Headers.hpp"
#include "../Request/Multipart.hpp"

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
	int mainState;
	int subState;
	string hold, key;
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
	Request_Fsm();
	~Request_Fsm();
};
