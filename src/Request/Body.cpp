#include "../../include/Request/Body.hpp"
#include "../../include/Server/Client.hpp"

int Body::multiPart(Client &client, string &buffer, ssize_t &size)
{
	unsigned char holdChar;

    // printf("parsheadelen = %ld countLength = %ld\n",client.request.ContentLength, countLength);
	countLength += size;
    if (isEncodChunk(client) == false && countLength > client.request.contentLength)
		return statu(client, "Error: Body::read size > client.request.ContentLength", 400);
	for (string::iterator it = buffer.begin(); it != buffer.end(); it++)
	{
		holdChar = *it;
		switch (client.request.subState)
		{
		case FIRST_BOUNDARY:
			if (count == sizeBoundary +1 && ("\r\n" + hold) == boundary + "\r")
			{
                if (holdChar == '\n')
                {
				    hold.clear();
				    count = 0;
				    client.request.subState = HANDLER_KEY;
                    continue;
                }
			}
			if (count > sizeBoundary +1)
				return statu(client, "Error: Body::read state START_BOUND", 400);
			count++;
            break;
		case AFTER_BOUNDARY:
			if (holdChar == '-')
			{
				client.request.subState = CHECK_END;
				break;
			}
			else if (holdChar == '\r')
			{
				client.request.subState = LF_BOUNDARY;
				hold.clear();
                continue;
			}
			return statu(client, "Error: Body::read state AFTER_BOUND", 400);
		case LF_BOUNDARY:
			if (holdChar == '\n')
			{
			    if (fileF != NULL)
			    {
			    	fclose(fileF);
			    	fileF = NULL;
			    }
			    client.request.subState = HANDLER_KEY;
                continue;
			}
			return statu(client, "Error: Body::read state BEFOR_KRY", 400);
		case HANDLER_KEY: // set max
			if (count >= MAX_KEY)
				return statu(client, "Error: Body::read state KEY MAX_KEY", 400);
			if (holdChar == ':')
			{
				client.request.subState = HANDLER_VALUE;
				count = 0;
				key = hold;
				hold.clear(); // store key
                continue;
			}
			else if (!isValidKey(holdChar))
				return statu(client, "Error: Body::read state HANDLER_KEY", 400);
			count++;
			break;
		case HANDLER_VALUE: // set max
			if (count >= MAX_VALUE)
				return statu(client, "Error: Body::read state VALUE MAX_VALUE", 400);
			if (holdChar == '\r')
			{
				File file;
				if (key == "Content-Disposition")
				{
                    int returnValue = createFile(client ,hold, file.fileName);
					if (returnValue != 0)
						return returnValue;
				}
				file.Content.insert(make_pair(key, trim(hold, " \t")));
				client.request.files.insert(client.request.files.begin(), file); // clear file ??
                if (client.request.files.size() > MAX_HEADERS)
				    return statu(client, "Error: Body::read state VALUE MAX_VALUE > MAX_HEADERS", 400);
				hold.clear();
				key.clear();
				client.request.subState = LF_VALUE;
				count = 0;
				continue;
			}
			count++;
			break;
		case LF_VALUE:
			if (holdChar == '\n')
			{
			    client.request.subState = CHECK_AFTER_VALUE;
			    hold.clear();
                continue;
			}
			return statu(client, "Error: Body::read state AFTER_VALUE", 400);
		case CHECK_AFTER_VALUE:
			if (holdChar == '\r')
			{
				client.request.subState = START_DATA;
                continue;
			}
			else if (isValidKey(holdChar))
			{
				client.request.subState = HANDLER_KEY;
			    break;
			}
			return statu(client, "Error: Body::read state CHECK", 400);
		case START_DATA:
			if (holdChar == '\n')
			{
			    hold.clear();
			    client.request.subState = WRITE_DATA;
                continue;
			}
			return statu(client, "Error: Body::read state AFTER_WRITE_DATA", 400);
		case WRITE_DATA:
        {
			if (count < sizeBoundary && holdChar == boundary[count])
			{
				count++;
				if (count == sizeBoundary)
					client.request.subState = AFTER_BOUNDARY;
			    break;
			}
			if (count == 0)
			{
				if (buffer.end() - it > sizeBoundary + 4)
				{
					size_t index = it - buffer.begin();
					size_t posBoundary = buffer.find(boundary, index);
					if (posBoundary == string::npos)
					{
                        if (fwrite(&buffer[index], 1, size - sizeBoundary - index, fileF) != (size - sizeBoundary - index))
                            return statu(client, "Error: fwrite fail", 500);
						it = buffer.begin() + (size - sizeBoundary - 1);
					}
					else
					{
                        if (fwrite(&buffer[index], 1, posBoundary - index, fileF) != (posBoundary - index))
                            return statu(client, "Error: fwrite fail", 500);
						it = buffer.begin() + posBoundary - 1;
					}
					continue;
				}
				if (fputc(holdChar, fileF) != holdChar)
                    return statu(client, "Error: fputc fail", 500);
				continue;
			}
            size_t holdSize = hold.size();
            if (fwrite(&hold[0], 1, holdSize, fileF) != holdSize)
                return statu(client, "Error: fwrite fail", 500);
			if (fputc(holdChar, fileF) != holdChar)
                return statu(client, "Error: fputc fail", 500);
			hold.clear();
			count = 0;
			continue;
        }
		case CHECK_END:
			if (holdChar == '-')
			{
				client.request.subState = CR_END;
                continue;
			}
			return statu(client, "Error: Body::read state CHECK_END", 400);
		case CR_END:
			if (holdChar == '\r')
			{
				client.request.subState = LF_END;
                continue;
			}
			return statu(client, "Error: Body::read state CR_END", 400);
		case LF_END: // check size of content length
			if (holdChar == '\n')
			{
                if ((isEncodChunk(client) == false && countLength == client.request.contentLength)
                || (isEncodChunk(client) == true && client.request.getEncodChunkState() == END_LAST_HEXA))
                {
                    if (fileF != NULL) {
                        fclose(fileF);
                        fileF = NULL;
                    }
                    client.request.setDeleteFiles(false);
                    return statu(client, "", 201);
                }
				hold.clear();
				return statu(client, "Error: Body::read LF_END countLength != client.request.contentLength", 400);
			}
			return statu(client, "Error: Body::read state LF_END", 400);
		}
		hold += holdChar;
	}
    return statu(client, "", 0);
}

int Body::writeBody(Client &client, string &buffer, ssize_t &size)
{
	if (fwrite(&buffer[0], 1, size, fileF) != (size_t)size)
        return statu(client, "Error: fwrite fail", 500);
	countLength += size;
	printf("counlen = %ld len = %ld\n", client.request.contentLength, countLength);
	if ((isEncodChunk(client) == false && countLength == client.request.contentLength)
    || (isEncodChunk(client) == true && client.request.getEncodChunkState() == END_LAST_HEXA))
	{
		fclose(fileF);
        fileF = NULL;
        client.request.setDeleteFiles(false);
        if (client.response.isCgi)
            return statu(client, "", 200);
        return statu(client, "", 201);
	}
	if (isEncodChunk(client) == false && countLength > client.request.contentLength)
		return statu(client, "countLength > client.request.ContentLength", 400);
    return statu(client, "", 0);
}

int Body::skipBody(Client &client, ssize_t &size)
{
	countLength += size;
        // printf("size = %ld\n", size);
	if ((isEncodChunk(client) == false && countLength == client.request.contentLength)
    || (isEncodChunk(client) == true && client.request.getEncodChunkState() == END_LAST_HEXA))
	{
        if(!client.response.method_allowd)
            return statu(client, "Error: Headers::checkrequest client.response.method_allowd", 405);
        if (client.request.Method == "DELETE")
            return statu(client, "",deleteMthod(client));
        return statu(client, "", 200);
	}
	if (isEncodChunk(client) == false && countLength > client.request.contentLength)
		return statu(client, "countLength > client.request.ContentLength", 400);
    return statu(client, "", 0);
}

// int Body::deleteMthod(Client &client)
// {
//     try {
//         std::string root = client.response.fullpath;
//         std::cout << "route : " << client.response.fullpath << std::endl;  /////!!!!!
//         isCanBeRemoved(client.response.fullpath);
//         removeDirfolder(client.response.fullpath, root);
//     }
//     catch(int returnValue)
//     {
//         client.request.setErrorMsg("");
//         return returnValue;
//     }
//     return 200;
// }

int Body::createFile(Client &client, const string &value, string &fileName)
{
	size_t fileNamePos = value.find("filename=");

	if (fileNamePos == string::npos)
		return statu(client, "Body::createFile filename", 400);
	fileName = trim(value.substr(fileNamePos + sizeof("filename")), " \"");
    fileName = getUploadPath(client) + fileName;
    // printf("fileName = %s\n", &fileName[0]);//////


    /*
    struct stat stats;
    if (stat(fileName.c_str(), &stats) == 0)
    {
        printf("Body::createFile file alredy exist= \n");
        return 400;
    }
    */

	fileF = fopen(fileName.c_str(), "w");
	if (!fileF)
		return statu(client, "Body::createFile fail to open file", 500);
	return 0;
}

bool Body::RandomFile(Request &Request, const string& path, const string& extension)
{
	string randomName;

    while (true) {
        randomName = "file-" + intToString(time(NULL));
        randomName = path + randomName + extension;
        // printf("randomName = %s\n", &randomName[0]);
	    ifstream inf(randomName.c_str());
        if (inf.is_open() == false)
            break ;
    }
	fileF = fopen(randomName.c_str(), "w");
	if (!fileF)
	{
		// printf("error Body::CGI fopen\n");
		return false;
	}
	File file;
	file.fileName = randomName;
	Request.files.push_back(file);
	return true;
}

int Body::statu(Client &client, const string& errorMsg, int statu)
{
    client.request.setReqstDone(statu);
    client.request.setErrorMsg(errorMsg);
    return statu;
}

bool Body::isEncodChunk(Client &client)
{
    return client.request.getDecodeFlag();
}

void Body::setBoundary(const string & boundary)
{
    this->boundary = boundary;
}

string Body::getBoundary(void) const
{
    return boundary;
}

void Body::setSizeBoundary(const int & sizeBoundary)
{
    this->sizeBoundary = sizeBoundary;
}

int Body::getSizeBoundary(void) const
{
    return sizeBoundary;
}

void Body::reset()
{
	count = 2;
	countLength = 0;
    if (fileF != NULL) {
        fclose(fileF);
    }
	fileF = NULL;
    sizeBoundary = 0;
    key.clear();
    hold.clear();
    boundary.clear();
}

Body::Body()
{
	count = 2;
	countLength = 0;
    sizeBoundary = 0;
	fileF = NULL;
}

Body::~Body() {}
