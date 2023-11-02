#include "../../include/Request/RequestLine.hpp"
#include "../../include/Request/Request_FSM.hpp"
// return done || error
// continue skip witout adding
// break add
// !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~

void RequestLine::read(Request_Fsm &Request, string &buffer, ssize_t &size)
{
	unsigned char character;
	// std::cout <<buffer << std::endl;
	for (string::iterator it = buffer.begin(); it != buffer.end(); it++)
	{
		character = *it;
		switch (Request.subState)
		{
		case REQUEST:
			if (character == 'G')
				method = GET;
			else if (character == 'P')
				method = POST;
			else if (character == 'D')
				method = DELETE;
			else
			{
				printf("Error: RequestLine::read subState REQUEST i = %ld c = %c\n", it - buffer.begin(), character);
				Request.ReqstDone = 400;
				return;
			}
			Request.subState = METHOD;
			count++;
			continue;
		case METHOD:
			if (character == PossiblMethod[method][count])
				count++;
			else
			{
				printf("Error: RequestLine::read subState METHOD i = %ld c = %c\n", it - buffer.begin(), character);
				Request.ReqstDone = 400;
				return;
			}
			if (count == PossiblMethod.find(method)->first)
			{
				Request.Method = PossiblMethod[method];
				Request.subState = METHOD_SPACE;
			}
			break;
		case METHOD_SPACE:
			if (character != ' ')
			{
				printf("Error: RequestLine::read subState METHOD_SPACE i = %ld c = %c\n", it - buffer.begin(), character);
				Request.ReqstDone = 400;
				return;
			}
			Request.subState = _URI;
			count = 0;
			Request.hold.clear();
			continue;
		case _URI:
			if (character == ' ')
			{
				Request.subState = VERSION;
				Request.URI = Request.hold;
				count = 0;
				Request.hold.clear();
				continue;
			}
			else if (!ValidURI(character))
			{
				printf("Error: RequestLine::read subState URI ValidURI i = %ld c = %c\n", it - buffer.begin(), character);
				Request.ReqstDone = 400;
				return;
			}
			else if (count > MAX_URI)
			{
				printf("Error: RequestLine::read subState URI count > max_lenURI i = %ld c = %c\n", it - buffer.begin(), character);
				Request.ReqstDone = 400;
				return;
			}
			count++;
			break;
		case VERSION:
			if (character == "HTTP/1.1"[count])
				count++;
			else
			{
				printf("Error: RequestLine::read subState VERSION i = %ld count = %d c = %c\n", it - buffer.begin(), count, character);
				Request.ReqstDone = 400;
				return;
			}
			if (count == 8)
			{
				Request.subState = CR;
				count = 0;
				Request.hold.clear();
				continue;
			}
			break;
		case CR:
			if (character != '\r')
			{
				printf("Error: RequestLine::read CR i = %ld c = %d\n", it - buffer.begin(), character);
				Request.ReqstDone = 400;
				return;
			}
			Request.subState = LF;
			continue;
		case LF:
			if (character != '\n')
			{
				printf("Error: RequestLine::read NL i = %ld c = %d\n", it - buffer.begin(), character);
				Request.ReqstDone = 400;
				return;
			}
			Request.mainState = HEADERS;
			Request.subState = CHECK;
			// printf("main = %d sub = %d\n", Request.mainState, Request.subState);
			buffer.erase(0, it - buffer.begin() + 1);
			size -= it - buffer.begin() + 1;
			Request.hold.clear();
			return;
		}
		Request.hold += character;
		// cout << "hold = "<< hold <<"\n";
	}
}

void RequestLine::reset()
{
	method = 0;
	count = 0;
}

RequestLine::RequestLine()
{
	PossiblMethod[GET] = "GET";
	PossiblMethod[POST] = "POST";
	PossiblMethod[DELETE] = "DELETE";
	count = 0;
}

RequestLine::~RequestLine() {}
