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
			return statu(client, "Error: Headers::read state CHECK", 400);
		case KEY: // set max
			if (count >= MAX_KEY)
				return statu(client, "Error: Headers::read state KEY MAX_KEY", 400);
			if (holdChar == ':')
			{
				key = strToLower(hold);
				hold.clear(); // store key
				client.request.subState = VALUE;
				count = 0;
				continue;
			}
			else if (!isValidKey(holdChar))
				return statu(client ,"Error: Headers::read state KEY", 400);
			count++;
			break;
		case VALUE: // set max
			if (count >= MAX_VALUE)
				return statu(client, "Error: Headers::read state VALUE MAX_VALUE", 400);
			if (holdChar == '\r')
			{
				client.request.mapHeaders.insert(make_pair(key, trim(hold, " \t")));
				hold.clear();
				key.clear();
                if (client.request.mapHeaders.size() > MAX_HEADERS)
				    return statu(client, "Error: Headers::read state VALUE MAX_HEADERS ", 400);
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
			return statu(client ,"Error: Headers::read state END_VALUE", 400);
		case END_HEADERS:
			if (holdChar == '\n')
			{
				buffer.erase(0, it - buffer.begin() + 1);
				size -= it - buffer.begin() + 1;
				hold.clear();
				return requestChecker(client);
			}
			return statu(client, "Error: Headers::read state END_HEADER", 400);
		}
		hold += holdChar;
		// printf(" i = %ld count = %d  char = |%c| hold = |%s|\n",i,count,c, hold.c_str());
		// std::cout << "hold = "<< hold <<"\n";
	}
    return 0;
}

int Headers::requestChecker(Client &client)
{
    client.response.startResponse(client);
    if(!client.response.method_allowd)
        return statu(client, "Error: Headers::checkrequest client.response.method_allowd", 405);
	if (client.request.Method == "GET")
		return 200;
    if (client.request.Method == "DELETE")
        return deleteMthod(client);
    int returnValue;

    if ((returnValue = transEncodChecker(client)) != 0)
        return returnValue;
    if (client.request.getDecodeFlag() == false
    && (returnValue = contentLenChecker(client)) != 0)
        return returnValue;
    if (client.response.isCgi)
		return (client.request.setErrorMsg(""), cgiChecker(client));
	multimap<string, string>::iterator it;
	it = client.request.mapHeaders.find("content-type"); // Content-Type: Body/form-data; boundary=- ???
    if (it == client.request.mapHeaders.end())
        return statu(client, "Error: Headers::checkrequest there is no Content-Type", 415);
    if (it ->second.find("multipart/form-data") != string::npos) {
        if ((returnValue = multiPartChecker(client, it->second)) != 0)
            return returnValue;
    }
    else if ((returnValue = mimeTypeChecker(client, it->second)) != 0)
        return returnValue;
    return 0;
}

int Headers::transEncodChecker(Client &client)
{
    multimap<string, string>::iterator it;

    it = client.request.mapHeaders.find("transfer-encoding");
	if (it != client.request.mapHeaders.end())
	{
		if (it->second != "chunked")
			return statu(client, "Error: Headers::checkrequest it->second != \"chunked\"", 501);
		client.request.setDecodeFlag(true);
	}
    return 0;
}

int Headers::contentLenChecker(Client &client)
{
    stringstream stream;
    multimap<string, string>::iterator it;

    it = client.request.mapHeaders.find("content-length");
	if (client.request.getDecodeFlag() == false && it == client.request.mapHeaders.end())
		return statu(client, "error Headers::checkrequest no shuncked no Content-Length", 411);
	else if (client.request.getDecodeFlag() == true && it == client.request.mapHeaders.end())
		return 0;
	stream << it->second;
	stream >> client.request.contentLength;
	if (!isDigit(it->second) || stream.fail())
		return statu(client, "error Headers::checkrequest ContentLength", 400);
    if (client.request.contentLength == 0)
        return 200;
    stringstream stream1(client.response.Config->LimitClientBodySize);
    size_t limitClientBodySize, diskSpace;
    stream1 >> limitClientBodySize;
    if (limitClientBodySize < client.request.contentLength) // add left space here
        return statu(client, "error Headers::checkContentLen client.request.contentLength > limitClientBodySize client body size", 413);
    if (getDiskSpace(getUploadPath(client), diskSpace) == false
    || diskSpace <= client.request.contentLength)
        return statu(client, "error Headers::checkContentLen diskSpace", 400);
    return 0;
}

int Headers::cgiChecker(Client &client)
{
    if (client.request.openBodyFile("/tmp/.", "") == false)
		return statu(client, "error Headers::MimeTypeChecker cgiChecker", 500);
    client.request.mainState = CGI;
    return 0;
}

int Headers::multiPartChecker(Client &client, string& value)
{
    size_t posBoundary = value.find("boundary="); //if boudry vid or not in correct form ??
	if (posBoundary == string::npos)
		return statu(client, "error Headers::checkrequest Content-Type boundary", 400);
	client.request.setBoundary("\r\n--" + value.substr(posBoundary + sizeof("boundary")));
	client.request.setSizeBoundary(client.request.getBoundary().size());
    client.request.mainState = MultiPart;
    client.request.subState = FIRST_BOUNDARY;
    return 0;
}

int Headers::mimeTypeChecker(Client &client, string& value)
{
    string extension = client.serverConf.DefaultServerConfig.mime.getExtensionMimeType(value);

    if (client.request.openBodyFile(getUploadPath(client), "." + extension) == false)
		return statu(client, "error Headers::MimeTypeChecker openBodyFile", 500);
    client.request.mainState = MIMETYPES;
    // printf("Headers::MimeTypeChecker\n");
    return 0;
}

int Headers::deleteMthod(Client &client)
{
    try {
        std::string root = client.response.fullpath;
        std::cout << "route : " << client.response.fullpath << std::endl;  /////!!!!!
        isCanBeRemoved(client.response.fullpath);
        removeDirfolder(client.response.fullpath, root);
    }
    catch(int returnValue)
    {
        client.request.setErrorMsg("");
        return returnValue;
    }
    return 200;
}

int Headers::statu(Client &client, const string& errorMsg, int statu)
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
}

Headers::Headers() : count(0) {}

Headers::~Headers() {}
