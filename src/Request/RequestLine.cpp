#include "../../include/Request/RequestLine.hpp"
#include "../../include/Request/Request_FSM.hpp"
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
				return 400;
			}
			client.request.subState = METHOD;
			count++;
			continue;
		case METHOD:
			if (character == PossiblMethod[method][count])
				count++;
			else
			{
				printf("Error: RequestLine::read subState METHOD i = %ld c = %c\n", it - buffer.begin(), character);
				return 400;
			}
			if (count == PossiblMethod.find(method)->first)
			{
				client.request.Method = PossiblMethod[method];
				client.request.subState = METHOD_SPACE;
			}
			break;
		case METHOD_SPACE:
			if (character != ' ')
			{
				printf("Error: RequestLine::read subState METHOD_SPACE i = %ld c = %c\n", it - buffer.begin(), character);
				return 400;
			}
			client.request.subState = _URI;
			count = 0;
			client.request.hold.clear();
			continue;
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
				return 400;
			}
			count++;
			break;
		case VERSION:
			if (character == "HTTP/1.1"[count])
				count++;
			else
			{
				printf("Error: RequestLine::read subState VERSION i = %ld count = %d c = %c\n", it - buffer.begin(), count, character);
				return 400;
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
				return 400;
			}
			client.request.subState = LF;
			continue;
		case LF:
			if (character != '\n')
			{
				printf("Error: RequestLine::read NL i = %ld c = %d\n", it - buffer.begin(), character);
				return 400;
			}
			// printf("main = %d sub = %d\n", client.request.mainState, client.request.subState);
			buffer.erase(0, it - buffer.begin() + 1);
			size -= it - buffer.begin() + 1;
			client.request.hold.clear();

			client.request.subState = CHECK;
			client.request.mainState = HEADERS;
			return 0;
		}
		client.request.hold += character;
		// cout << "hold = "<< hold <<"\n";
	}
    return 0;
}

// void RequestLine::checker(Client &client)
// {
// 	std::cout << "▻merge Headers Values◅ -------------------------------------------" << std::endl;
// 	// mergeHeadersValues(client);
// 	// std::cout << "▻Get config◅ -----------------------------------------------------" << std::endl;
// 	 client.response.getConfig(client);
// 	// std::cout << "▻Parse uri◅ ------------------------------------------------------" << std::endl;
// 	 client.response.parseUri(client.request.URI);
// 	// std::cout << "▻Get full path◅ --------------------------------------------------" << std::endl;
// 	 client.response.getFULLpath();
// 	if ( client.response.responseDone)
// 		return;
// 	// std::cout << "▻Regenerate extension◅ -------------------------------------------" << std::endl;
// 	client.response.regenerateExtonsion();
// 	if ( client.response.responseDone)
// 		return;
// }

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
