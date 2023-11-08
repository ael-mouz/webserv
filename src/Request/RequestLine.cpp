#include "../../include/Request/RequestLine.hpp"
// #include "../../include/Request/Request_FSM.hpp"
#include "../../include/Server/Client.hpp"
// return done || error
// continue skip witout adding
// break add
// !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~

int RequestLine::read(Client &client, string &buffer, ssize_t &size)
{
	unsigned char character;
	// std::cout <<buffer << std::endl;
	for (string::iterator it = buffer.begin(); it != buffer.end(); it++)
	{
		character = *it;
		switch (client.request.subState)
		{
		case METHOD:
            if ((client.request.hold == "GET" || client.request.hold == "POST" || client.request.hold == "DELETE")
                && character == ' ')
            {
                count = 0;
                client.request.Method = client.request.hold;
                client.request.hold.clear();
                client.request.subState = _URI;
                continue;
            }
            if (count < 3 && character == "GET"[count])
                ;
            else if (count < 4 && character == "POST"[count])
                ;
            else if (count < 6 && character == "DELETE"[count])
                ;
            else {
                printf("Error: RequestLine::read subState REQUEST i = %ld c = %c count = %d\n", it - buffer.begin(), character, count);
				return 405;
            }
            count++;
            break;
		case _URI:
			if (character == ' ')
			{
				client.request.subState = VERSION;
				client.request.URI = client.request.hold;
				count = 0;
				client.request.hold.clear();
				continue;
			}
			else if (!ValidURI(character))
			{
				printf("Error: RequestLine::read subState URI ValidURI i = %ld c = %c\n", it - buffer.begin(), character);
				return 400;
			}
			else if (count > MAX_URI)
			{
				printf("Error: RequestLine::read subState URI count > max_lenURI i = %ld c = %c\n", it - buffer.begin(), character);
				return 414;
			}
			count++;
			break;
		case VERSION:
			if (character == "HTTP/1.1"[count])
				count++;
			else
			{
				printf("Error: RequestLine::read subState VERSION i = %ld count = %d c = %c\n", it - buffer.begin(), count, character);
				return 505;
			}
			if (count == 8)
			{
				client.request.subState = CR;
				count = 0;
				client.request.hold.clear();
				continue;
			}
			break;
		case CR:
			if (character != '\r')
			{
				printf("Error: RequestLine::read CR i = %ld c = %d\n", it - buffer.begin(), character);
				return 505;
			}
			client.request.subState = LF;
			continue;
		case LF:
			if (character != '\n')
			{
				printf("Error: RequestLine::read NL i = %ld c = %d\n", it - buffer.begin(), character);
				return 505;
			}
			// printf("main = %d sub = %d\n", client.request.mainState, client.request.subState);
			buffer.erase(0, it - buffer.begin() + 1);
			size -= it - buffer.begin() + 1;
			client.request.hold.clear();

			client.request.subState = CHECK;
			client.request.mainState = HEADERS;
			return checker(client);
		}
		client.request.hold += character;
		// cout << "hold = "<< hold <<"\n";
	}
    return 0;
}

int RequestLine::checker(Client &client)
{
	client.response.getConfig(client);
	client.response.parseUri(client.request.URI);
	client.response.getRoute();
	client.response.genrateRederiction();
	if (client.response.responseDone)
		return 200;
	client.response.getFULLpath();
	if (client.response.responseDone)
		return 200;
	client.response.regenerateExtonsion();
	if (client.response.responseDone)
		return 200;
    if(client.response.isCgi)
        client.request.isCGI = true;
    return 0;
}

void RequestLine::reset()
{
	count = 0;
}

RequestLine::RequestLine()
{
	count = 0;
}

RequestLine::~RequestLine() {}
