#include "../../include/Request/ChunkedEncoding.hpp"
#include "../../include/Request/Request_FSM.hpp"

void ChunkedEncoding::read(Request_Fsm &Request, string &buffer, ssize_t &size)
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
				Request.mainState = 400;
				return;
			}
			hold += character;
			buffer.erase(it);
			continue;
		case LF_AFTER_HEXA:
			if (character != '\n')
			{
				printf("Error: ChunkedEncoding::read state LF_FIRST_HEXA i = %ld c = %d\n", it - buffer.begin(), character);
				Request.mainState = 400;
				return;
			}
			countLength = HexaToDicimal(hold);
			hold.clear();
			buffer.erase(it);
			decodeState = SKIP_BODY;
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
				printf("Error: ChunkedEncoding::read state CR_BEFOR_HEXA i = %ld c = %d\n", it - buffer.begin(), character);
				Request.mainState = 400;
				return;
			}
			decodeState = LF_BEFOR_HEXA;
			buffer.erase(it);
			continue;
		case LF_BEFOR_HEXA:
			if (character != '\n')
			{
				printf("Error: ChunkedEncoding::read state LF_BEFOR_HEXA i = %ld c = %d\n", it - buffer.begin(), character);
				Request.mainState = 400;
				return;
			}
			decodeState = HEXA;
			buffer.erase(it);
			continue;
		}
		it++;
	}
	size = buffer.size();
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
