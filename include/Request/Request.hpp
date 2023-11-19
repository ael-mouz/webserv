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
	multimap<string, string> Content;
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

    void setBoundary(const string& boundary);
    string getBoundary(void) const;
    void setSizeBoundary(const int& sizeBoundary);
    int getSizeBoundary(void) const;
    void setDeleteFiles(const bool& deleteFiles);
    bool getDeleteFiles(void) const;
    void setDecodeFlag(const bool& decodeFlag);
    bool getDecodeFlag(void) const;
    void setTimeLastData(const size_t& timeLastData);
    size_t getTimeLastData(void) const;
    // void setContentLength(const size_t& contentLength);
    // size_t getContentLength(void) const;
    void setReqstDone(const int& ReqstDone);
    int getReqstDone(void) const;
    void setErrorMsg(const string& errorMsg);
    string getErrorMsg(void) const;

	void read(Client &client, string &buffer, ssize_t &size);
	void reset(void);
    bool openBodyFile(const string &path, const string &extension);
    int getEncodChunkState(void);
	Request();
	~Request();
};
