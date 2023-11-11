#pragma once

#include "../Server/Utils.hpp"

class Request;

enum chunkedEncoding
{
	HEXA,
	LF_AFTER_HEXA,
	SKIP_BODY,
	CR_BEFOR_HEXA,
	LF_BEFOR_HEXA,
};

class ChunkedEncoding
{
private:
	size_t count;
	int decodeState;
	string hold;
	size_t countLength;

public:
	void read(Request &Request, string &buffer, ssize_t &size);
	void reset();
	ChunkedEncoding();
	~ChunkedEncoding();
};
