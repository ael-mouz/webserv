#include "../../include/Request/RequestLine.hpp"
#include "../../include/Request/Request.hpp"
// return done || error
// continue skip witout adding
// break add
// !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~

int RequestLine::read(Request &request, string &buffer, ssize_t &size) //change client to request
{
	unsigned char holdChar;

	for (string::iterator it = buffer.begin(); it != buffer.end(); it++)
	{
		holdChar = *it;
		switch (request.subState)
		{
		case METHOD:
            if ((hold == "GET" || hold == "POST" || hold == "HEAD"
                || hold == "DELETE") && holdChar == ' ')
            {
                count = 0;
                request.Method = hold;
                hold.clear();
                request.subState = _URI;
                continue;
            }
            if (count < 3 && holdChar == "GET"[count])
                ;
            else if (count < 4 && holdChar == "POST"[count])
                ;
            else if (count < 4 && holdChar == "HEAD"[count])
                ;
            else if (count < 6 && holdChar == "DELETE"[count])
                ;
            else {
				return request.setErrorMsg("Error: RequestLine::read subState REQUEST"), 405;
            }
            count++;
            break;
		case _URI:
			if (count > MAX_URI)
				return request.setErrorMsg("Error: RequestLine::read subState URI count > max_lenURI"), 414;
			if (holdChar == ' ')
			{
				request.subState = VERSION;
				request.URI = hold;
				count = 0;
				hold.clear();
				continue;
			} else if (holdChar == '%') {
				request.subState = DECODE_URI;
                continue;
            } else if (!isValidURI(holdChar)) {
				return request.setErrorMsg("Error: RequestLine::read subState URI ValidURI"), 400;
			}
			count++;
			break;
		case DECODE_URI:
            if (std::isxdigit(holdChar) && count_hexa < 2)
            {
                hold += holdChar;
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
			return request.setErrorMsg("Error: RequestLine::read subState DECODE_URI"), 414;
		case VERSION:
			if (count < 10 && holdChar == "HTTP/1.1\r\n"[count])
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
			    break;
            }
	    	return request.setErrorMsg("Error: RequestLine::read subState VERSION"), 505;
		}
		hold += holdChar;
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
