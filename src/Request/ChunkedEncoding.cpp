#include "../../include/Request/ChunkedEncoding.hpp"
#include "../../include/Request/Request.hpp"

int ChunkedEncoding::read(Request &Request, string &buffer, ssize_t &size)
{
	unsigned char character;
    (void)Request;
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
				return 400;
			}
			hold += character;
			buffer.erase(it);
			continue;
		case LF_AFTER_HEXA:
			if (character != '\n')
			{
				printf("Error: ChunkedEncoding::read state LF_FIRST_HEXA i = %ld c = %d\n", it - buffer.begin(), character);
				return 400;
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
				return 400;
			}
			decodeState = LF_BEFOR_HEXA;
			buffer.erase(it);
			continue;
		case LF_BEFOR_HEXA:
			if (character != '\n')
			{
				printf("Error: ChunkedEncoding::read state LF_BEFOR_HEXA i = %ld c = %d\n", it - buffer.begin(), character);
				return 400;
			}
			decodeState = HEXA;
			buffer.erase(it);
			continue;
        case CR_END_CHUNKED:
            if (character != '\r')
            {
				printf("Error: ChunkedEncoding::read state CR_END_CHUNKED i = %ld c = %d\n", it - buffer.begin(), character);
				return 400;
            }
            decodeState = LF_END_CHUNKED;
			buffer.erase(it);
			continue;
        case LF_END_CHUNKED:
            if (character != '\n')
            {
				printf("Error: ChunkedEncoding::read state LF_END_CHUNKED i = %ld c = %d\n", it - buffer.begin(), character);
				return 400;
            }
			buffer.erase(it);
            /*if you exite here with return 0 the program can't check if there is
                other data after the end uf chunked \r\n0\r\n*/
			continue;
		}
		it++;
	}
	size = buffer.size();
    return 0;
}

void ChunkedEncoding::reset()
{
	decodeState = HEXA;
	countLength = 0;
	count = 0;
	hold.clear();
}

ChunkedEncoding::ChunkedEncoding()
{
	decodeState = HEXA;
	countLength = 0;
	count = 0;
}

ChunkedEncoding::~ChunkedEncoding() {}
