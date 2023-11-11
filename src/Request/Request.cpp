#include "../../include/Request/Request.hpp"
#include "../../include/Request/Body.hpp"
#include "../../include/Server/Client.hpp"


void Request::read(Client &client, string &buffer, ssize_t &size)
{
	if (mainState == REQUEST_LINE) {
		ReqstDone = requestLine.read(client, buffer, size);
		if (buffer.empty() || ReqstDone != 0)
			return;
    }
	if (mainState == HEADERS) {

		ReqstDone = headers.read(client, buffer, size);
		if (buffer.empty() || ReqstDone != 0)
			return;
    }
	if (decodeFlag)
		decode.read(*this, buffer, size);
    if (mainState == MultiPart) {
	    body.multiPart(client, buffer, size);
    } else if (mainState == CGI) {
		body.CGI(client, buffer, size);
    } else if (mainState == MIMETYPES) {
        body.mimeType(client, buffer, size);
    }
}

bool Request::openBodyFile(const string &path, const string &extension)
{
    return(body.RandomFile(*this, path, extension));
}

void Request::reset(void) // im smart
{
	// Request clear();
	hold.clear();
	key.clear();
	boundary.clear();
	Method.clear();
	URI.clear();
	mapHeaders.clear();
	for (vector<File>::iterator it = files.begin(); it != files.end(); it++)
	{
        if (deleteFiles == true)
        {
            std::remove(&it->fileName[0]); // can ramove an existing file
        }
		it->fileName.clear();
		it->Content.clear();
	}
	files.clear();
	mainState = METHOD;
	ReqstDone = 0;
	subState = 0;
	sizeBoundary = 0;
	ContentLength = 0;
	decodeFlag = false;
	isCGI = false;
    deleteFiles = true;
	requestLine.reset();
	headers.reset();
	body.reset();
	decode.reset();
	// *this = clear;
}

Request::Request()
{
	mainState = METHOD;
	ReqstDone = 0;
	subState = 0;
	decodeFlag = false;
	isCGI = false;
	sizeBoundary = 0;
	ContentLength = 0;
    deleteFiles = true;
}

Request::~Request() {}
