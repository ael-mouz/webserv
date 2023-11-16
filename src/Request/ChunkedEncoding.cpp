#include "../../include/Request/ChunkedEncoding.hpp"
#include "../../include/Server/Client.hpp"

int ChunkedEncoding::read(Client &client, string &buffer, ssize_t &size)
{
	unsigned char character;

	for (string::iterator it = buffer.begin(); it != buffer.end();)
	{
		character = *it;
		switch (decodeState)
		{
		case HEXA: // remove hexa
			if (character == '\r')
			{
				buffer.erase(it);
				decodeState = LF_AFTER_HEXA;
				continue;
			}
			else if (!isxdigit(character))
			{
				printf("Error: ChunkedEncoding::read state HEXA i = %ld c = %d\n", it - buffer.begin(), character);
				return client.request.setErrorMsg(""), 400;
			}
			hold += character;
			buffer.erase(it);
			continue;
		case LF_AFTER_HEXA:
			if (character != '\n')
			{
				printf("Error: ChunkedEncoding::read state LF_FIRST_HEXA i = %ld c = %d\n", it - buffer.begin(), character);
				return client.request.setErrorMsg(""), 400;
			}
			countLength = HexaToDicimal(hold);
			hold.clear();
			buffer.erase(it);
            // printf("countLength = %ld\n", countLength);
            decodeState = countLength != 0 ? SKIP_BODY : CR_END_CHUNKED;
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
				decodeState = CR_BEFOR_HEXA;
			}
			break;
		}
		case CR_BEFOR_HEXA:
			if (character != '\r')
			{
				printf("Error: ChunkedEncoding::read state CR_BEFOR_HEXA i = %ld c = %c\n", it - buffer.begin(), character);
				return client.request.setErrorMsg(""), 400;
			}
			decodeState = LF_BEFOR_HEXA;
			buffer.erase(it);
			continue;
		case LF_BEFOR_HEXA:
			if (character != '\n')
			{
				printf("Error: ChunkedEncoding::read state LF_BEFOR_HEXA i = %ld c = %d\n", it - buffer.begin(), character);
				return client.request.setErrorMsg(""), 400;
			}
			decodeState = HEXA;
			buffer.erase(it);
			continue;
        case CR_END_CHUNKED:
            if (character != '\r')
            {
				printf("Error: ChunkedEncoding::read state CR_END_CHUNKED i = %ld c = %d\n", it - buffer.begin(), character);
				return client.request.setErrorMsg(""), 400;
            }
            decodeState = LF_END_CHUNKED;
			buffer.erase(it);
			continue;
        case LF_END_CHUNKED:
            if (character != '\n')
            {
				printf("Error: ChunkedEncoding::read state LF_END_CHUNKED i = %ld c = %d\n", it - buffer.begin(), character);
				return client.request.setErrorMsg(""), 400;
            }
			buffer.erase(it);
            /*if you exite here with return 0 the program can't check if there is
                other data after the end uf chunked \r\n0\r\n*/
			continue;
		}
		it++;
	}
	size = buffer.size();
    totalSize += size;
    return client.request.setErrorMsg(""), totalSizeChecker(client, totalSize);
}

int ChunkedEncoding::totalSizeChecker(Client &client, size_t totalSize)
{
    stringstream stream1(client.response.Config->LimitClientBodySize);
    size_t limitClientBodySize, diskSpace;
    stream1 >> limitClientBodySize;
    if (limitClientBodySize < totalSize) // add left space here
    {
        printf("error ChunkedEncoding::totalSizeChecker totalSize > limitClientBodySize client body size\n");
        return client.request.setErrorMsg(""), 413;
    }
    if (getDiskSpace(getUploadPath(client), diskSpace) == false
    || diskSpace <= totalSize)
    {
        printf("error ChunkedEncoding::totalSizeChecker diskSpace \n");
        return client.request.setErrorMsg(""), 400;
    }
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
