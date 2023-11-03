#include "../../include/Request/Headers.hpp"
#include "../../include/Server/Client.hpp"

int Headers::read(Client &client, string &buffer, ssize_t &size)
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
				return 400;
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
				return 400;
			}
			else if (count >= MAX_KEY)
			{
				printf("Error: Headers::read state KEY MAX_KEY i = %d c = %c\n", count, character);
				return 400;
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
				return 400;
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
				return 400;
			}
			break;
		case END_HEADERS:
			if (character == '\n')
			{
				buffer.erase(0, it - buffer.begin() + 1);
				size -= it - buffer.begin() + 1;
				client.request.hold.clear();
				// checkrequest(client);
				client.request.subState = FIRST_BOUNDARY;
				client.request.mainState = BODY;
				// client.request.decode.reset();
				return checkrequest(client);
			}
			else
			{
				printf("Error: Headers::read state END_HEADER i = %ld c = %c\n", it - buffer.begin(), character);
				return 400;
			}
			break;
		}
		client.request.hold += character;
		// printf(" i = %ld count = %d  char = |%c| hold = |%s|\n",i,count,c, hold.c_str());
		// std::cout << "hold = "<< hold <<"\n";
	}
    return 0;
}

int Headers::checkrequest(Client &client)
{
	if (client.request.Method == "GET" )
		return 200;

	std::stringstream stream;
	std::multimap<std::string, std::string>::iterator itHeader;

	itHeader = client.request.mapHeaders.find("Transfer-Encoding");
	if (itHeader != client.request.mapHeaders.end())
	{
		if (itHeader->second != "chunked")
		{
			std::cout << "error request not implemnted";
			return 400;
		}
		client.request.decodeFlag = true;
		// return ;
	}
	itHeader = client.request.mapHeaders.find("Content-Length");
	if (itHeader == client.request.mapHeaders.end())
	{ // if you're in this scoup you should have all indormation about body
		std::cout << "error Headers::checkrequest no Content-Length \n";
		return 400;
	}
	stream << itHeader->second;
	stream >> client.request.ContentLength;
	if (!isDigit(itHeader->second) || stream.fail())
	{
		std::cout << "error Headers::checkrequest ContentLength\n";
		return 400;
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
		return 400;
	}
	client.request.boundary = "\r\n--" + itHeader->second.substr(posBoundary + sizeof("boundary"));
	client.request.sizeBoundary = client.request.boundary.size();
    return 0;
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
