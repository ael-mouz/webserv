#pragma once

#include "../Server/Utils.hpp"

class Request_Fsm;

enum multipartState
{
	FIRST_BOUNDARY,
	AFTER_BOUNDARY,
	LF_BOUNDARY,
	HANDLER_KEY,
	HANDLER_VALUE,
	LF_VALUE,
	CHECK_AFTER_VALUE,
	START_DATA,
	WRITE_DATA,
	CHECK_END,
	CR_END,
	LF_END,
};

class Multipart
{
private:
	FILE *fileF;
	int count;
	int writeToFile;

public:
	size_t countLength;
	void read(Request_Fsm &Request, string &buffer, ssize_t &size);
	void CGI(Request_Fsm &Request, string &buffer, ssize_t &size);
	void createFile(const string &value, std::string &fileName);
	bool createFileCGI(Request_Fsm &Request);
	void reset();
	Multipart();
	~Multipart();
};
