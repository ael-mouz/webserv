#include "../../include/Request/Request_FSM.hpp"
#include "../../include/Request/Multipart.hpp"

void Request_Fsm::read(string &buffer, ssize_t &size) // no need to bool
{
	if (mainState == REQUEST_LINE)
	{
		requestLine.read(*this, buffer, size); // can be a normal function or functtion inside classe
	}
	if (!buffer.empty() && mainState == HEADERS)
	{
		headers.read(*this, buffer, size); // can be a normal function or functtion inside classe
	}
	if (!buffer.empty() && mainState == BODY)
	{
		if (decodeFlag)
			decode.read(*this, buffer, size);

		if (!buffer.empty())
		{
			multi.read(*this, buffer, size);
			// multi.CGI(*this, buffer, size);
		}
	}
}

void Request_Fsm::clear(void) // im smart
{
	hold.clear();
	key.clear();
	boundary.clear();
	Method.clear();
    URI.clear();
	mapHeaders.clear();
	for (vector<File>::iterator it = files.begin(); it != files.end(); it++)
	{
		it->fileName.clear();
		it->Content.clear();
	}
	files.clear();
    mainState = REQUEST_LINE;
	ReqstDone = 0;
	subState = 0;
    sizeBoundary = 0;
    ContentLength = 0;
	decodeFlag = false;
    requestLine.reset();
    headers.reset();
    multi.reset();
    decode.reset();
}

Request_Fsm& Request_Fsm::operator=(const Request_Fsm& overl)
{
    requestLine = overl.requestLine;
    headers = overl.headers;
    multi = overl.multi;
    decode = overl.decode;
    mainState = overl.mainState;
    subState = overl.subState;
    hold = overl.hold;
    key = overl.key;
    boundary = overl.boundary;
    decodeFlag = overl.decodeFlag;
    sizeBoundary = overl.sizeBoundary;
    ContentLength = overl.ContentLength;
    ReqstDone = overl.ReqstDone;
    Method = overl.Method;
    URI = overl.URI;
    mapHeaders = overl.mapHeaders;
    files = overl.files;
    return *this;
}

Request_Fsm::Request_Fsm(const Request_Fsm& copy) : serverConf(copy.serverConf)
{
    *this = copy;
}

Request_Fsm::Request_Fsm(const ServerConf& serverConf) : serverConf(serverConf)
{
	mainState = REQUEST_LINE;
	ReqstDone = 0;
	subState = 0;
	decodeFlag = false;
    sizeBoundary = 0;
    ContentLength = 0;
}

Request_Fsm::~Request_Fsm() {}
