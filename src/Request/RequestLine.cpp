#include "../../include/Request/RequestLine.hpp"
#include "../../include/Request/Request.hpp"
// return done || error
// continue skip witout adding
// break add
// !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~

int RequestLine::read(Request &request, string &buffer, ssize_t &size) //change client to request
{
	unsigned char character;

	for (string::iterator it = buffer.begin(); it != buffer.end(); it++)
	{
		character = *it;
		switch (request.subState)
		{
		case METHOD:
            if ((hold == "GET" || hold == "POST" || hold == "HEAD"
                || hold == "DELETE") && character == ' ')
            {
                count = 0;
                request.Method = hold;
                hold.clear();
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
				return request.setErrorMsg(""), 405;
            }
            count++;
            break;
		case _URI:
			if (character == ' ')
			{
				request.subState = VERSION;
				request.URI = hold;
				count = 0;
				hold.clear();
				continue;
			} else if (character == '%')
            {
				request.subState = DECODE_URI;
                continue;
            }
			else if (!isValidURI(character))
			{
				printf("Error: RequestLine::read subState URI ValidURI i = %ld c = %c\n", it - buffer.begin(), character);
				return request.setErrorMsg(""), 400;
			}
			else if (count > MAX_URI)
			{
				printf("Error: RequestLine::read subState URI count > max_lenURI i = %ld c = %c\n", it - buffer.begin(), character);
				return request.setErrorMsg(""), 414;
			}
			count++;
			break;
		case DECODE_URI:
            if (std::isxdigit(character) && count_hexa < 2)
            {
                hold += character;
                count_hexa++;
                if (count_hexa == 2)
                {
                    char c = HexaToDicimal(hold.substr(hold.size() - 2));
                    hold.erase(hold.end() -2, hold.end());
                    hold += c;
                    count_hexa = 0;
                    request.subState = _URI;
                }
                continue;
            }
            else
            {
                printf("Error: RequestLine::read subState DECODE_URI i = %ld c = %c\n", it - buffer.begin(), character);
				return request.setErrorMsg(""), 414;
            }
		case VERSION:
			if (count < 10 && character == "HTTP/1.1\r\n"[count])
            {
				count++;
			    if (count == 10)
			    {
			    	count = 0;
			    	hold.clear();
                    buffer.erase(0, it - buffer.begin() + 1);
			        size -= it - buffer.begin() + 1;
			        request.subState = CHECK;
			        request.mainState = HEADERS;
			    	return request.setErrorMsg(""), 0;
			    }
            }
			else
			{
				printf("Error: RequestLine::read subState VERSION i = %ld count = %d c = %c\n", it - buffer.begin(), count, character);
				return request.setErrorMsg(""), 505;
			}
			break;
		}
		hold += character;
	}
    return request.setErrorMsg(""), 0;
}

void RequestLine::reset()
{
	count = 0;
    count_hexa = 0;
    hold.clear();
}

RequestLine::RequestLine() : count(0), count_hexa(0) {}

RequestLine::~RequestLine() {}
