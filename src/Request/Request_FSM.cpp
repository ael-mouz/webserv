#include "../../include/Request/Request_FSM.hpp"
#include "../../include/Request/Multipart.hpp"
#include "../../include/Server/Client.hpp"


void Request_Fsm::read(Client &client, string &buffer, ssize_t &size)
{
	switch (mainState) {
		case REQUEST_LINE:
			ReqstDone = requestLine.read(client, buffer, size);
			if (buffer.empty() || ReqstDone != 0)
				break;
		case HEADERS:
			ReqstDone = headers.read(client, buffer, size);
			if (buffer.empty() || ReqstDone != 0)
				break;
		case BODY:
			if (decodeFlag)
				decode.read(*this, buffer, size);
			if (isCGI)
			{
				multi.CGI(*this, buffer, size);
				break;
			}
			multi.read(*this, buffer, size);
	}
}

void Request_Fsm::clear(void) // im smart
{
	// Request_Fsm clear();
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
	isCGI = false;
	requestLine.reset();
	headers.reset();
	multi.reset();
	decode.reset();
	// *this = clear;
}

Request_Fsm::Request_Fsm()
{
	mainState = REQUEST_LINE;
	ReqstDone = 0;
	subState = 0;
	decodeFlag = false;
	isCGI = false;
	sizeBoundary = 0;
	ContentLength = 0;
}

Request_Fsm::~Request_Fsm() {}
