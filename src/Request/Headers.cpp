#include "../../include/Request/Headers.hpp"
#include "../../include/Request/Request_FSM.hpp"

void Headers::read(Request_Fsm &Request, string &buffer, ssize_t &size)
{
	unsigned char character;

	for (string::iterator it = buffer.begin(); it != buffer.end(); it++)
	{
		character = *it;
		switch (Request.subState)
		{
		case CHECK:
			if (character == '\r')
				Request.subState = END_HEADERS;
			else if (ValidKey(character))
				Request.subState = KEY;
			else
			{
				printf("Error: Headers::read state CHECK i = %ld c = %c\n", it - buffer.begin(), character);
				Request.mainState = 0;
				return;
			}
			break;
		case KEY: // set max
			if (character == ':')
			{
				Request.key = Request.hold;
				Request.hold.clear(); // store key
				Request.subState = VALUE;
				count = 0;
				continue;
			}
			else if (!ValidKey(character))
			{
				printf("Error: Headers::read state KEY i = %ld c = %c\n", it - buffer.begin(), character);
				Request.mainState = 0;
				return;
			}
			else if (count >= MAX_KEY)
			{
				printf("Error: Headers::read state KEY MAX_KEY i = %d c = %c\n", count, character);
				Request.mainState = 0;
				return;
			}
			count++;
			break;
		case VALUE: // set max
			if (character == '\r')
			{
				Request.mapHeaders.insert(make_pair(Request.key, trim(Request.hold, " \t")));
				Request.hold.clear();
				Request.key.clear();
				Request.subState = END_VALUE;
				count = 0;
				continue;
			}
			if (count >= MAX_VALUE)
			{
				printf("Error: Headers::read state VALUE MAX_VALUE i = %d c = %c\n", count, character);
				Request.mainState = 0;
				return;
			}
			count++;
			break;
		case END_VALUE:
			if (character == '\n')
			{
				Request.subState = CHECK;
				continue;
			}
			else
			{
				printf("Error: Headers::read state END_VALUE i = %ld c = %c\n", it - buffer.begin(), character);
				Request.mainState = 0;
				return;
			}
			break;
		case END_HEADERS:
			if (character == '\n')
			{
				checkrequest(Request);
				buffer.erase(0, it - buffer.begin() + 1);
				size -= it - buffer.begin() + 1;
				Request.hold.clear();
				Request.subState = START_BOUND;
				Request.mainState = BODY;
				// Request.decode.reset();
				return;
			}
			else
			{
				printf("Error: Headers::read state END_HEADER i = %ld c = %c\n", it - buffer.begin(), character);
				Request.mainState = 0;
				return;
			}
			break;
		}
		Request.hold += character;
		// printf(" i = %ld count = %d  char = |%c| hold = |%s|\n",i,count,c, hold.c_str());
		// std::cout << "hold = "<< hold <<"\n";
	}
}

void Headers::checkrequest(Request_Fsm &Request)
{
	if (Request.Method == "GET")
	{
		Request.ReqstDone = true;
		return;
	}
	if (Request.Method != "POST")
	{
		// Request.ReqstDone = 0;
		return;
	}

	std::stringstream stream;
	std::multimap<std::string, std::string>::iterator itHeader;

	itHeader = Request.mapHeaders.find("Transfer-Encoding");
	if (itHeader != Request.mapHeaders.end())
	{
		if (itHeader->second != "chunked")
		{
			std::cout << "error request not implemnted";
			return;
		}
		Request.decodeFlag = true;
		// return ;
	}
	itHeader = Request.mapHeaders.find("Content-Length");
	Request.mainState = 0;
	if (itHeader == Request.mapHeaders.end())
	{ // if you're in this scoup you should have all indormation about body
		std::cout << "error Headers::checkrequest no Content-Length \n";
		Request.mainState = 0;
		return;
	}
	stream << itHeader->second;
	stream >> Request.ContentLength;
	if (!isDigit(itHeader->second) || stream.fail())
	{
		std::cout << "error Headers::checkrequest ContentLength\n";
		Request.mainState = 0;
		return;
	}
	// if (Request.ContentLength == 0) {
	//     std::cout << "error Headers::checkrequest ContentLength == 0\n"; Request.mainState = 0;
	//     return ;
	// }
	itHeader = Request.mapHeaders.find("Content-Type"); // Content-Type: multipart/form-data; boundary=- ???
	size_t posBoundary = itHeader->second.find("boundary=");
	if (itHeader == Request.mapHeaders.end() || posBoundary == std::string::npos)
	{ // if multipart/form-data || multipart/mixed || multipart/related || multipart/alternative
		std::cout << "error Headers::checkrequest Content-Type\n";
		Request.mainState = 0;
		return;
	}
	std::string tmp;

	tmp = "\r\n--" + itHeader->second.substr(posBoundary + sizeof("boundary"));
	// std::cout << "boundary  " << tmp << "\n";
	Request.sizeBoundary = tmp.size();
	// Request.boundary = tmp.c_str();
	// printf("char = %d\n", Request.boundary[0]);
	Request.boundary = tmp;
	// std::cout << "boundary  " << Request.sizeBoundary << "\n";
	Request.mainState = BODY;
	// Request.state = START_BOUND; multipart/form-data
}

void Headers::reset()
{
    count = 0;
}

Headers::Headers()
{
	// state = CHECK;
	count = 0;
}

Headers::~Headers() {}
