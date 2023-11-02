#include "../../include/Request/Headers.hpp"
#include "../../include/Server/Client.hpp"

void Headers::read(Client &client, string &buffer, ssize_t &size)
{
	unsigned char character;

	for (string::iterator it = buffer.begin(); it != buffer.end(); it++)
	{
		character = *it;
		switch (client.request.subState)
		{
		case CHECK:
			if (character == '\r')
				client.request.subState = END_HEADERS;
			else if (ValidKey(character))
				client.request.subState = KEY;
			else
			{
				printf("Error: Headers::read state CHECK i = %ld c = %c\n", it - buffer.begin(), character);
				client.request.ReqstDone = 400;
				return;
			}
			break;
		case KEY: // set max
			if (character == ':')
			{
				client.request.key = client.request.hold;
				client.request.hold.clear(); // store key
				client.request.subState = VALUE;
				count = 0;
				continue;
			}
			else if (!ValidKey(character))
			{
				printf("Error: Headers::read state KEY i = %ld c = %c\n", it - buffer.begin(), character);
				client.request.ReqstDone = 400;
				return;
			}
			else if (count >= MAX_KEY)
			{
				printf("Error: Headers::read state KEY MAX_KEY i = %d c = %c\n", count, character);
				client.request.ReqstDone = 400;
				return;
			}
			count++;
			break;
		case VALUE: // set max
			if (character == '\r')
			{
				client.request.mapHeaders.insert(make_pair(client.request.key, trim(client.request.hold, " \t")));
				client.request.hold.clear();
				client.request.key.clear();
				client.request.subState = END_VALUE;
				count = 0;
				continue;
			}
			if (count >= MAX_VALUE)
			{
				printf("Error: Headers::read state VALUE MAX_VALUE i = %d c = %c\n", count, character);
				client.request.ReqstDone = 400;
				return;
			}
			count++;
			break;
		case END_VALUE:
			if (character == '\n')
			{
				client.request.subState = CHECK;
				continue;
			}
			else
			{
				printf("Error: Headers::read state END_VALUE i = %ld c = %c\n", it - buffer.begin(), character);
				client.request.ReqstDone = 400;
				return;
			}
			break;
		case END_HEADERS:
			if (character == '\n')
			{
				checkrequest(client);
				buffer.erase(0, it - buffer.begin() + 1);
				size -= it - buffer.begin() + 1;
				client.request.hold.clear();
				client.request.subState = START_BOUND;
				client.request.mainState = BODY;
				// client.request.decode.reset();
				return;
			}
			else
			{
				printf("Error: Headers::read state END_HEADER i = %ld c = %c\n", it - buffer.begin(), character);
				client.request.ReqstDone = 400;
				return;
			}
			break;
		}
		client.request.hold += character;
		// printf(" i = %ld count = %d  char = |%c| hold = |%s|\n",i,count,c, hold.c_str());
		// std::cout << "hold = "<< hold <<"\n";
	}
}

void Headers::checkrequest(Client &client)
{
	if (client.request.Method == "GET")
	{
		// puts("ss");
		client.request.ReqstDone = 200;
		return;
	}
	if (client.request.Method != "POST")
	{
		// client.request.ReqstDone = 0;
		return;
	}

	std::stringstream stream;
	std::multimap<std::string, std::string>::iterator itHeader;

	itHeader = client.request.mapHeaders.find("Transfer-Encoding");
	if (itHeader != client.request.mapHeaders.end())
	{
		if (itHeader->second != "chunked")
		{
			std::cout << "error request not implemnted";
			return;
		}
		client.request.decodeFlag = true;
		// return ;
	}
	itHeader = client.request.mapHeaders.find("Content-Length");
	client.request.mainState = 0;
	if (itHeader == client.request.mapHeaders.end())
	{ // if you're in this scoup you should have all indormation about body
		std::cout << "error Headers::checkrequest no Content-Length \n";
		client.request.mainState = 0;
		return;
	}
	stream << itHeader->second;
	stream >> client.request.ContentLength;
	if (!isDigit(itHeader->second) || stream.fail())
	{
		std::cout << "error Headers::checkrequest ContentLength\n";
		client.request.mainState = 0;
		return;
	}
	// if (client.request.ContentLength == 0) {
	//     std::cout << "error Headers::checkrequest ContentLength == 0\n"; client.request.mainState = 0;
	//     return ;
	// }
	itHeader = client.request.mapHeaders.find("Content-Type"); // Content-Type: multipart/form-data; boundary=- ???
	size_t posBoundary = itHeader->second.find("boundary=");
	if (itHeader == client.request.mapHeaders.end() || posBoundary == std::string::npos)
	{ // if multipart/form-data || multipart/mixed || multipart/related || multipart/alternative
		std::cout << "error Headers::checkrequest Content-Type\n";
		client.request.mainState = 0;
		return;
	}
	std::string tmp;

	tmp = "\r\n--" + itHeader->second.substr(posBoundary + sizeof("boundary"));
	// std::cout << "boundary  " << tmp << "\n";
	client.request.sizeBoundary = tmp.size();
	// client.request.boundary = tmp.c_str();
	// printf("char = %d\n", client.request.boundary[0]);
	client.request.boundary = tmp;
	// std::cout << "boundary  " << client.request.sizeBoundary << "\n";
	client.request.mainState = BODY;
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
