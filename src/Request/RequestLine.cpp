#include "../../include/Request/RequestLine.hpp"
#include "../../include/Server/Client.hpp"
#include "../../include/Request/Request.hpp"
// return done || error
// continue skip witout adding
// break add
// !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~

int RequestLine::read(Request &request, string &buffer, ssize_t &size) //change client to request
{
	unsigned char character;
	// std::cout <<buffer << std::endl;
	for (string::iterator it = buffer.begin(); it != buffer.end(); it++)
	{
		character = *it;
		switch (request.subState)
		{
		case METHOD:
			if ((request.hold == "GET" || request.hold == "POST" || request.hold == "DELETE" || request.hold == "HEAD")
			&& character == ' ')
			{
                count = 0;
                request.Method = request.hold;
                request.hold.clear();
                request.subState = _URI;
                continue;
            }
            if (count < 3 && character == "GET"[count])
                ;
            else if (count < 4 && character == "POST"[count])
                ;
            else if (count < 4 && character == "HEAD"[count])
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
				request.subState = VERSION;
				request.URI = request.hold;
                // std::printf("URI %s\n", request.URI.c_str());
				count = 0;
				request.hold.clear();
				continue;
			} else if (character == '%')
            {
				request.subState = DECODE_URI;
                continue;
            }
			else if (!isValidURI(character))
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
		case DECODE_URI:
            if (std::isxdigit(character) && count_hexa < 2)
            {
                request.hold += character;
                count_hexa++;
                if (count_hexa == 2)
                {
                    char c = HexaToDicimal(request.hold.substr(request.hold.size() - 2));
                    // std::cout << HexaToDicimal(request.hold.substr(request.hold.size() - 2)) << std::endl;
                    request.hold.erase(request.hold.end() -2, request.hold.end());
                    request.hold += c;
                    count_hexa = 0;
                    request.subState = _URI;
                }
                continue;
            }
            else
            {
                printf("Error: RequestLine::read subState DECODE_URI i = %ld c = %c\n", it - buffer.begin(), character);
				return 400;
            }
		case VERSION:
			if (count < 10 && character == "HTTP/1.1\r\n"[count])
            {
				count++;
			    if (count == 10)
			    {
			    	count = 0;
			    	request.hold.clear();
                    buffer.erase(0, it - buffer.begin() + 1);
			        size -= it - buffer.begin() + 1;
			        request.subState = CHECK;
			        request.mainState = HEADERS;
			    	return 0;
			    }
            }
			else
			{
				printf("Error: RequestLine::read subState VERSION i = %ld count = %d c = %c\n", it - buffer.begin(), count, character);
				return 505;
			}
			break;
		}
		request.hold += character;
		// cout << "hold = "<< hold <<"\n";
	}
    return 0;
}

void RequestLine::reset()
{
	count = 0;
}

RequestLine::RequestLine() : count(0), count_hexa(0) {}

RequestLine::~RequestLine() {}
