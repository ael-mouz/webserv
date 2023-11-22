#include "../../include/Request/RequestLine.hpp"
#include "../../include/Request/Request.hpp"

int RequestLine::read(Request &request, string &buffer, ssize_t &size)
{
	unsigned char holdChar;

	for (string::iterator it = buffer.begin(); it != buffer.end(); it++)
	{
		holdChar = *it;
		switch (request.subState)
		{
		case METHOD:
			if ((hold == "GET" || hold == "POST" || hold == "DELETE" || hold == "HEAD") && holdChar == ' ')
			{
				count = 0;
				request.Method = hold;
				hold.clear();
				request.subState = _URI;
				continue;
			} else if ((hold == "PUT" || hold == "OPTIONS" || hold == "TRACE" || hold == "CONNECT" || hold == "PATCH") && holdChar == ' ') {
				count = 0;
				request.Method = hold;
				hold.clear();
                return request.setErrorMsg( "Method not allowed"), 405;
            }
			if (count < 3 && holdChar == "GET"[count])
				;
			else if (count < 4 && holdChar == "POST"[count])
				;
			else if (count < 6 && holdChar == "DELETE"[count])
				;
			else if (count < 4 && holdChar == "HEAD"[count])
				;
			else if (count < 3 && holdChar == "PUT"[count])
				;
			else if (count < 7 && holdChar == "OPTIONS"[count])
				;
			else if (count < 5 && holdChar == "TRACE"[count])
				;
			else if (count < 7 && holdChar == "CONNECT"[count])
				;
			else if (count < 6 && holdChar == "PATCH"[count])
				;
			else
			{
				return request.setErrorMsg("Unknown method"), 400;
			}
			count++;
			break;
		case _URI:
			if (count > MAX_URI)
				return request.setErrorMsg("URI too long"), 414;
			if (holdChar == ' ')
			{
				request.subState = VERSION;
				if (hold.empty())
					return request.setErrorMsg("Empty URI"), 400;
				request.URI = hold;
				count = 0;
				hold.clear();
				continue;
			}
			else if (holdChar == '%')
			{
				request.subState = DECODE_URI;
				continue;
			}
			else if (!isValidURI(holdChar))
			{
				return request.setErrorMsg("Invalid URI character"), 400;
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
					hold.erase(hold.end() - 2, hold.end());
					hold += c;
					count_hexa = 0;
					request.subState = _URI;
				}
				continue;
			}
			return request.setErrorMsg("Invalid decoded URI"), 400;
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
			return request.setErrorMsg("Invalid HTTP version"), 505;
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
