#include "../../include/Request/Request_FSM.hpp"
#include "../../include/Request/Multipart.hpp"
#include "../../include/Server/Client.hpp"

void Request_Fsm::read(Client &client, string &buffer, ssize_t &size)
{
	if (mainState == REQUEST_LINE)
	{
		// std::cout <<buffer << std::endl;
		requestLine.read(*this, buffer, size);
	}
	// printf("%s\n", &Method[0]);
	if (!buffer.empty() && mainState == HEADERS)
	{
		headers.read(client, buffer, size);
	}
	if (!buffer.empty() && mainState == BODY)
	{
		if (decodeFlag)
			decode.read(*this, buffer, size);

		if (!buffer.empty())
		{
			// multi.read(*this, buffer, size);
			multi.CGI(*this, buffer, size);
		}
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
	sizeBoundary = 0;
	ContentLength = 0;
}

Request_Fsm::~Request_Fsm() {}
