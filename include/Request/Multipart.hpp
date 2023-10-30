#pragma once
#include "../Utils.hpp"

class Request_Fsm;

enum multipartState
{
	START_BOUND,
	AFTER_BOUND,
	LF_BOUNDRY,
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
	size_t countLength;

public:
	void read(Request_Fsm &Request, string &buffer, ssize_t &size);
	void CGI(Request_Fsm &Request, string &buffer, ssize_t &size);
	void createFile(const string &value, std::string &fileName);
	bool createFileCGI(Request_Fsm &Request);
	Multipart();
	~Multipart();
};
