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
	Request_Fsm clear;

	hold.clear();
	key.clear();
	Method.clear();
	mapHeaders.clear();
	boundary.clear();
	for (vector<File>::iterator it = files.begin(); it != files.end(); it++)
	{
		it->fileName.clear();
		it->Content.clear();
	}
	files.clear();
	*this = clear;
}

Request_Fsm::Request_Fsm()
{
	mainState = REQUEST_LINE;
	ReqstDone = 0;
	subState = 0;
	decodeFlag = false;
}

Request_Fsm::~Request_Fsm() {}
