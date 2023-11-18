#include "../../include/Request/ChunkedEncoding.hpp"
#include "../../include/Server/Client.hpp"

int ChunkedEncoding::read(Client &client, string &buffer, ssize_t &size)
{
	unsigned char holdChar;

	for (string::iterator it = buffer.begin(); it != buffer.end();)
	{
		holdChar = *it;
		switch (decodeState)
		{
		case HEXA:
			if (holdChar == '\r')
			{
				buffer.erase(it);
				decodeState = LF_AFTER_HEXA;
				continue;
			}
			if (!isxdigit(holdChar))
				return statu(client, "Invalid hexadecimal number", 400);
			hold += holdChar;
			buffer.erase(it);
			continue;
		case LF_AFTER_HEXA:
			if (holdChar != '\n' || hold.empty())
				return statu(client, "Invalid chunked syntax", 400);
			countLength = HexaToDicimal(hold);
			hold.clear();
			buffer.erase(it);
            decodeState = countLength != 0 ? SKIP_BODY : END_LAST_HEXA;
			continue;
		case SKIP_BODY:
		{
			size_t remainingLen = buffer.end() - it;
			if (remainingLen <= (countLength - count))
			{
				it = buffer.end() - 1;
				count += remainingLen;
			}
			else
			{
				it = buffer.begin() + ((it - buffer.begin()) + countLength - count - 1);
				count = 0;
				decodeState = BEFOR_HEXA;
			}
			break;
		}
        case BEFOR_HEXA:
            if (holdChar != "\r\n"[count])
				return statu(client, "Invalid CRLF (Carriage Return + Line Feed) before hexadecimal", 400);
			buffer.erase(it);
            count++;
            if (count == 2 && !(count = 0))
			    decodeState = HEXA;
			continue;
        case END_LAST_HEXA:
            if (count > 2 || holdChar != "\r\n"[count])
				return statu(client, "Invalid chunked body", 400);
			buffer.erase(it);
            count++;
			continue;
		}
		it++;
	}
	size = buffer.size();
    totalSize += size;
    return totalSizeChecker(client, totalSize);
}

int ChunkedEncoding::totalSizeChecker(Client &client, size_t totalSize)
{
    stringstream stream1(client.response.Config->LimitClientBodySize);
    size_t limitClientBodySize, diskSpace;
    stream1 >> limitClientBodySize;
    if (limitClientBodySize < totalSize)
        return statu(client, "Body too large", 413);
    if (client.request.Method == "POST" && (getDiskSpace(getUploadPath(client), diskSpace) == false
    || diskSpace <= totalSize))
        return statu(client, "No space left", 400);
    return 0;
}

void ChunkedEncoding::setDecodeState(const int& decodeState)
{
    this->decodeState = decodeState;
}

int ChunkedEncoding::getDecodeState(void) const
{
    return decodeState;
}

int ChunkedEncoding::statu(Client &client, const string& errorMsg, int statu)
{
    client.request.setReqstDone(statu);
    client.request.setErrorMsg(errorMsg);
    return statu;
}

void ChunkedEncoding::reset()
{
	decodeState = HEXA;
	countLength = 0;
	count = 0;
    totalSize = 0;
	hold.clear();
}

ChunkedEncoding::ChunkedEncoding()
{
	decodeState = HEXA;
	countLength = 0;
	count = 0;
    totalSize = 0;
}

ChunkedEncoding::~ChunkedEncoding() {}
