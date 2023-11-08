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
				client.request.key = strToLower(client.request.hold);
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
	if (client.request.Method == "GET" || client.request.Method == "DELETE")
		return 200;
	multimap<string, string>::iterator it;

	it = client.request.mapHeaders.find("transfer-encoding");
	if (it != client.request.mapHeaders.end())
	{
		if (it->second != "chunked")
		{
			printf("error request not implemnted\n");
			return 501;
		}
		client.request.decodeFlag = true;
	}
    int returnValue = ContentLenChecker(client);
    if (returnValue != 0)
	{
        return returnValue;
	}
	it = client.request.mapHeaders.find("content-type"); // Content-Type: multipart/form-data; boundary=- ???
    if (it == client.request.mapHeaders.end())
    {
        printf("Error: Headers::checkrequest there is no Content-Type\n");
        return 400;
    }
    if (it ->second.find("multipart/form-data") != string::npos) {
        if ((returnValue = MultiPartChecker(client, it->second)) != 0)// if multipart/form-data || multipart/mixed || multipart/related || multipart/alternative
            return returnValue;
    }
    else if ((returnValue = MimeTypeChecker(client, it->second)) != 0)
        return returnValue;
    return 0;
}

int Headers::ContentLenChecker(Client &client)
{
    stringstream stream;
    multimap<string, string>::iterator it;

    it = client.request.mapHeaders.find("content-length");
	if (client.request.decodeFlag == false && it == client.request.mapHeaders.end())
	{
		printf("error Headers::checkrequest no shuncked no Content-Length \n");
		return 411;
	}
	else if (client.request.decodeFlag == true && it == client.request.mapHeaders.end())
   	{
		return 0;
	}
	stream << it->second;
	stream >> client.request.ContentLength;
	if (!isDigit(it->second) || stream.fail())
	{
		printf("error Headers::checkrequest ContentLength\n");
		return 400;
	}
    if (client.request.ContentLength == 0)
    {
        printf("error Headers::checkContentLen len = 0\n");
        return 200;
    }
    stringstream stream1(client.response.Config->LimitClientBodySize);
    size_t limitClientBodySize;
    stream1 >> limitClientBodySize;
    if (client.request.ContentLength > limitClientBodySize)
    {
        printf("error Headers::checkContentLen client.request.ContentLength > limitClientBodySize client body size\n");
        return 413;
    }
    return 0;
}

int Headers::MultiPartChecker(Client &client, string& value)
{
    size_t posBoundary = value.find("boundary="); //if boudry vid or not in correct form ??
	if (posBoundary == string::npos)
	{
		cout << "error Headers::checkrequest Content-Type\n";
		return 400;
	}
	client.request.boundary = "\r\n--" + value.substr(posBoundary + sizeof("boundary"));
	client.request.sizeBoundary = client.request.boundary.size();
    return 0;
}

int Headers::MimeTypeChecker(Client &client, string& value)
{
    (void)client;
    (void)value;
    printf("Headers::MimeTypeChecker\n");
    return 200;
}


void Headers::reset()
{
	count = 0;
}

Headers::Headers() : count(0) {}

Headers::~Headers() {}
