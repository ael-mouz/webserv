#include "../../include/Request/Headers.hpp"
#include "../../include/Server/Client.hpp"

int Headers::read(Client &client, string &buffer, ssize_t &size)
{
	unsigned char holdChar;

	for (string::iterator it = buffer.begin(); it != buffer.end(); it++)
	{
		holdChar = *it;
		switch (client.request.subState)
		{
		case CHECK:
			if (isValidKey(holdChar))
			{
				client.request.subState = KEY;
				break;
			}
			else if (holdChar == '\r' && !client.request.mapHeaders.empty())
			{
				client.request.subState = END_HEADERS;
				break;
			}
			return statu(client, "Invalid headers syntax", 400);
		case KEY:
			if (count >= MAX_KEY)
				return statu(client, "Header key too long", 400);
			if (holdChar == ':')
			{
				key = strToLower(hold);
				hold.clear();
				client.request.subState = VALUE;
				count = 0;
				continue;
			}
			else if (!isValidKey(holdChar))
				return statu(client, "Invalid header key character", 400);
			count++;
			break;
		case VALUE:
			if (count >= MAX_VALUE)
				return statu(client, "Header value too long", 400);
			if (holdChar == '\r')
			{
				client.request.mapHeaders.insert(make_pair(key, trim(hold, " \t")));
				hold.clear();
				key.clear();
				if (client.request.mapHeaders.size() > MAX_HEADERS)
					return statu(client, "Too many headers", 400);
				client.request.subState = END_VALUE;
				count = 0;
				continue;
			}
			count++;
			break;
		case END_VALUE:
			if (holdChar == '\n')
			{
				client.request.subState = CHECK;
				continue;
			}
			return statu(client, "Header value must end with LF (Line Feed)", 400);
		case END_HEADERS:
			if (holdChar == '\n')
			{
				buffer.erase(0, it - buffer.begin() + 1);
				size -= it - buffer.begin() + 1;
				hold.clear();
				return requestChecker(client);
			}
			return statu(client, "Headers must end with LF (Line Feed)", 400);
		}
		hold += holdChar;
	}
	return 0;
}

int Headers::requestChecker(Client &client)
{
	int returnValue;
	client.response.startResponse(client);
	if ((returnValue = contentTypeChecker(client)) != 0)
		return returnValue;
	if ((returnValue = transEncodChecker(client)) != 0)
		return returnValue;
	if (client.request.getDecodeFlag() == false && (returnValue = contentLenChecker(client)) != 0)
		return returnValue;
	if (client.request.Method != "POST")
	{
		if (isContentTypeInHeaders == true && isContentLengthInHeaders == false)
			return statu(client, "Unknown body length", 411);
		if ((client.request.getDecodeFlag() == false && client.request.contentLength > 0) ||
			(client.request.getDecodeFlag() == true))
		{
			client.request.mainState = _SKIP_BODY;
			return 0;
		}
		if (client.request.Method == "DELETE")
			return deleteMthod(client);
		else if (client.request.Method != "GET")
			return statu(client, "Method not allowed", 405);
		return 200;
	}
	if (client.response.isCgi)
		return (cgiChecker(client));
	return 0;
}

int Headers::transEncodChecker(Client &client)
{
	multimap<string, string>::iterator it;

	it = client.request.mapHeaders.find("transfer-encoding");
	if (it != client.request.mapHeaders.end())
	{
		if (it->second != "chunked")
			return statu(client, "Transfer-Encoding must be 'chunked'", 501);
		client.request.setDecodeFlag(true);
	}
	return 0;
}

int Headers::contentLenChecker(Client &client)
{
	stringstream stream;
	multimap<string, string>::iterator it;

	it = client.request.mapHeaders.find("content-length");
	if (client.request.Method != "POST" && it == client.request.mapHeaders.end())
		return isContentLengthInHeaders = false, 0;
	else if (it == client.request.mapHeaders.end())
		return statu(client, "Unknown body length", 411);
	stream << it->second;
	stream >> client.request.contentLength;
	if (!isDigit(it->second) || stream.fail())
		return statu(client, "Invalid Content-Length", 400);
	if (client.request.contentLength == 0)
		return 200;
	stringstream stream1(client.response.Config->LimitClientBodySize);
	size_t limitClientBodySize, diskSpace;
	stream1 >> limitClientBodySize;
	if (client.request.Method == "POST" && limitClientBodySize < client.request.contentLength)
		return statu(client, "Body too large", 413);
	if (client.request.Method == "POST")
	{
		if (getDiskSpace(getUploadPath(client), diskSpace) == false)
			return statu(client, "Upload file not found", 500);
		if (diskSpace <= client.request.contentLength)
			return statu(client, "No space left", 507);
	}
	return isContentLengthInHeaders = true, 0;
}

int Headers::contentTypeChecker(Client &client)
{
	int returnValue;

	multimap<string, string>::iterator it;
	it = client.request.mapHeaders.find("content-type");
	if (client.request.Method == "POST")
	{
		if (it == client.request.mapHeaders.end())
			return statu(client, "No Content-Type provided", 415);
		if (it->second.find("multipart/form-data") != string::npos)
		{
			if ((returnValue = multiPartChecker(client, it->second)) != 0)
				return returnValue;
		}
		else if ((returnValue = mimeTypeChecker(client, it->second)) != 0)
			return returnValue;
	}
	else
		isContentTypeInHeaders = (it == client.request.mapHeaders.end()) ? false : true;
	return 0;
}

int Headers::cgiChecker(Client &client)
{
	if (client.request.openBodyFile("/tmp/.", "") == false)
		return statu(client, "Generate file failed for cgi", 500);
	client.request.mainState = CGI;
	return 0;
}

int Headers::multiPartChecker(Client &client, string &value)
{
	size_t posBoundary = value.find("boundary=");
	if (posBoundary == string::npos)
		return statu(client, "No boundary provided", 400);
	string boundary = value.substr(posBoundary + sizeof("boundary"));
	if (boundary.empty())
		return statu(client, "Empty boundary", 400);
	client.request.setBoundary("\r\n--" + boundary);
	client.request.setSizeBoundary(client.request.getBoundary().size());
	client.request.mainState = MultiPart;
	client.request.subState = FIRST_BOUNDARY;
	return 0;
}

int Headers::mimeTypeChecker(Client &client, string &value)
{
	string newExtension, extension = client.serverConf.DefaultServerConfig.mime.getExtensionMimeType(value);
	newExtension = (extension.empty()) ? extension : "." + extension;
	if (client.request.openBodyFile(getUploadPath(client), newExtension) == false)
		return statu(client, "Generate file failed for MimeType", 500);
	client.request.mainState = MIMETYPES;
	return 0;
}

int Headers::statu(Client &client, const string &errorMsg, int statu)
{
	client.request.setReqstDone(statu);
	client.request.setErrorMsg(errorMsg);
	return statu;
}

void Headers::reset()
{
	count = 0;
	key.clear();
	hold.clear();
	isContentTypeInHeaders = false;
	isContentLengthInHeaders = false;
}

Headers::Headers() : count(0), isContentTypeInHeaders(false), isContentLengthInHeaders(false) {}

Headers::~Headers() {}
