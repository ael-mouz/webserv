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
	MultiPart,
	CGI,
	MIMETYPES,
	_SKIP_BODY,
};

struct File
{
	string fileName;
	bool fileExists;
};

class Request
{
private:
	RequestLine requestLine;
	Headers headers;
	ChunkedEncoding decode;
	Body body;
	bool deleteFiles;
	bool decodeFlag;
	size_t timeLastData;
	string errorMsg;

public:
	int ReqstDone;
	int mainState;
	int subState;
	size_t contentLength;
	string Method;
	string URI;
	multimap<string, string> mapHeaders;
	vector<File> files;

	void setBoundary(const string &boundary);
	void setSizeBoundary(const int &sizeBoundary);
	void setTimeLastData(const size_t &timeLastData);
	void setDeleteFiles(const bool &deleteFiles);
	void setReqstDone(const int &ReqstDone);
	void setErrorMsg(const string &errorMsg);
	string getBoundary(void) const;
	int getSizeBoundary(void) const;
	size_t getTimeLastData(void) const;
	bool getDeleteFiles(void) const;
	void setDecodeFlag(const bool &decodeFlag);
	bool getDecodeFlag(void) const;
	int getReqstDone(void) const;
	string getErrorMsg(void) const;
	void read(Client &client, string &buffer, ssize_t &size);
	void reset(void);
	bool openBodyFile(const string &path, const string &extension);
	int getEncodChunkState(void);
	Request();
	~Request();
};
