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
    CR_END_CHUNKED,
    LF_END_CHUNKED,
};

class ChunkedEncoding
{
private:
	size_t count;
	string hold;
	size_t countLength;
    size_t totalSize;
	int decodeState;
    int totalSizeChecker(Client &client, size_t totalSize);

public:
    void setDecodeState(const int& decodeState);
    int getDecodeState(void) const;
	int read(Client &client, string &buffer, ssize_t &size);
	void reset();
	ChunkedEncoding();
	~ChunkedEncoding();
};
