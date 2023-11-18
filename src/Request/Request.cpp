#include "../../include/Request/Request.hpp"
#include "../../include/Request/Body.hpp"
#include "../../include/Server/Client.hpp"

void Request::read(Client &client, string &buffer, ssize_t &size)
{
	if (mainState == REQUEST_LINE)
	{
		ReqstDone = requestLine.read(*this, buffer, size);
		if (buffer.empty() || ReqstDone != 0)
			return;
	}
	if (mainState == HEADERS)
	{
		ReqstDone = headers.read(client, buffer, size);
		if (buffer.empty() || ReqstDone != 0)
			return;
	}
	if (decodeFlag == true && (ReqstDone = decode.read(client, buffer, size)) != 0)
		return;
	if (mainState == MultiPart)
	{
		body.multiPart(client, buffer, size);
	}
	else if (mainState == MIMETYPES || mainState == CGI)
	{
		body.writeBody(client, buffer, size);
	}
	else if (mainState == _SKIP_BODY)
	{
		body.skipBody(client, size);
	}
}

bool Request::openBodyFile(const string &path, const string &extension)
{
	return (body.RandomFile(*this, path, extension));
}

int Request::getEncodChunkState()
{
	return decode.getDecodeState();
}

void Request::setBoundary(const string &boundary)
{
	this->body.setBoundary(boundary);
}

string Request::getBoundary(void) const
{
	return this->body.getBoundary();
}

void Request::setSizeBoundary(const int &sizeBoundary)
{
	this->body.setSizeBoundary(sizeBoundary);
}

int Request::getSizeBoundary(void) const
{
	return this->body.getSizeBoundary();
}

void Request::setDeleteFiles(const bool &deleteFiles)
{
	this->deleteFiles = deleteFiles;
}

bool Request::getDeleteFiles(void) const
{
	return deleteFiles;
}

void Request::setDecodeFlag(const bool &decodeFlag)
{
	this->decodeFlag = decodeFlag;
}

bool Request::getDecodeFlag(void) const
{
	return decodeFlag;
}

void Request::setTimeLastData(const size_t &timeLastData)
{
	this->timeLastData = timeLastData;
}

size_t Request::getTimeLastData(void) const
{
	return timeLastData;
}

void Request::setReqstDone(const int &ReqstDone)
{
	this->ReqstDone = ReqstDone;
}

int Request::getReqstDone(void) const
{
	return ReqstDone;
}

void Request::setErrorMsg(const string &errorMsg)
{
	this->errorMsg = errorMsg;
}

string Request::getErrorMsg(void) const
{
	return errorMsg;
}

void Request::reset(void)
{
	Method.clear();
	URI.clear();
	mapHeaders.clear();
	for (vector<File>::iterator it = files.begin(); it != files.end(); it++)
	{
		if (deleteFiles == true)
			std::remove(&it->fileName[0]);
		it->fileName.clear();
		it->Content.clear();
	}
	files.clear();
	mainState = METHOD;
	ReqstDone = 0;
	subState = 0;
	contentLength = 0;
	decodeFlag = false;
	deleteFiles = true;
	timeLastData = 0;
	requestLine.reset();
	headers.reset();
	body.reset();
	decode.reset();
}

Request::Request()
{
	mainState = METHOD;
	ReqstDone = 0;
	subState = 0;
	decodeFlag = false;
	contentLength = 0;
	deleteFiles = true;
	timeLastData = 0;
}

Request::~Request() {}
