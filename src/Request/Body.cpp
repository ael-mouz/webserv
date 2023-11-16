#include "../../include/Request/Body.hpp"
#include "../../include/Server/Client.hpp"

void Body::multiPart(Client &client, string &buffer, ssize_t &size)
{
	unsigned char character;

	countLength += size;
    // printf("parsheadelen = %ld countLength = %ld\n",client.request.ContentLength, countLength);
    if (isEncodChunk(client) == false && countLength > client.request.contentLength)
    {
        setError(client, "Error: Body::read size > client.request.ContentLength", 400);
		return;
    }
	for (string::iterator it = buffer.begin(); it != buffer.end(); it++)
	{
		character = *it;
		switch (client.request.subState)
		{
		case FIRST_BOUNDARY:
			if (count == sizeBoundary +1 && ("\r\n" + hold) == boundary + "\r")
			{
                if (character == '\n')
                {
				    hold.clear();
				    count = 0;
				    client.request.subState = HANDLER_KEY;
                    continue;
                }
			}
			if (count > sizeBoundary +1)
			{
                setError(client, "Error: Body::read state START_BOUND", 400);
				return;
			}
			count++;
            break;
		case AFTER_BOUNDARY: // case if there is only one final boundary
			if (character == '-')
			{
				client.request.subState = CHECK_END;
				break;
			}
			else if (character == '\r')
			{
				client.request.subState = LF_BOUNDARY;
				hold.clear();
                continue;
			}
            setError(client, "Error: Body::read state AFTER_BOUND", 400);
			return;
		case LF_BOUNDARY:
			if (character == '\n')
			{
			    if (fileF != NULL)
			    {
			    	fclose(fileF);
			    	fileF = NULL;
			    }
			    client.request.subState = HANDLER_KEY;
                continue;
			}
            setError(client, "Error: Body::read state BEFOR_KRY", 400);
			return;
		case HANDLER_KEY: // set max
			if (character == ':')
			{
				client.request.subState = HANDLER_VALUE;
				count = 0;
				key = hold;
				hold.clear(); // store key
                continue;
			}
			else if (!isValidKey(character))
			{
                setError(client, "Error: Body::read state HANDLER_KEY", 400);
				return;
			}
			else if (count >= MAX_KEY)
			{
                setError(client, "Error: Body::read state KEY MAX_KEY", 400);
				return;
			}
			count++;
			break;
		case HANDLER_VALUE: // set max
			if (character == '\r')
			{
				File file;
				if (key == "Content-Disposition")
				{
                    int returnValue = createFile(client ,hold, file.fileName);
					if (returnValue != 0)
						return ;
				}
				file.Content.insert(make_pair(key, trim(hold, " \t")));
				client.request.files.insert(client.request.files.begin(), file); // clear file ??
				hold.clear();
				key.clear();
				client.request.subState = LF_VALUE;
				count = 0;
				continue;
			}
			if (count >= MAX_VALUE)
			{
                setError(client, "Error: Body::read state VALUE MAX_VALUE", 400);
				return;
			}
			count++;
			break;
		case LF_VALUE:
			if (character == '\n')
			{
			    client.request.subState = CHECK_AFTER_VALUE;
			    hold.clear();
                continue;
			}
            setError(client, "Error: Body::read state AFTER_VALUE", 400);
			return;
		case CHECK_AFTER_VALUE:
			if (character == '\r')
			{
				client.request.subState = START_DATA;
                continue;
			}
			else if (isValidKey(character))
			{
				client.request.subState = HANDLER_KEY;
			    break;
			}
            setError(client, "Error: Body::read state CHECK", 400);
			return;
		case START_DATA:
			if (character == '\n')
			{
			    hold.clear();
			    client.request.subState = WRITE_DATA;
                continue;
			}
            setError(client, "Error: Body::read state AFTER_WRITE_DATA", 400);
			return;
		case WRITE_DATA:
			if (count < sizeBoundary && character == boundary[count])
			{
				count++;
				if (count == sizeBoundary)
					client.request.subState = AFTER_BOUNDARY;
			}
			else
			{
				if (count == 0)
				{
					if (buffer.end() - it > sizeBoundary + 4)
					{
						size_t index = it - buffer.begin();
						size_t posBoundary = buffer.find(boundary, index);
						if (posBoundary == string::npos)
						{

							size_t ret = fwrite(&buffer[index], 1, size - sizeBoundary - index, fileF);
                            if (ret != (size - sizeBoundary - index))
                                setError(client, "Error: fwrite fail", 500);
                            // printf("ret  = %ld size - %ld\n", ret, size - sizeBoundary - index);
							it = buffer.begin() + (size - sizeBoundary - 1);
						}
						else
						{
							fwrite(&buffer[index], 1, posBoundary - index, fileF);
							it = buffer.begin() + posBoundary - 1;
						}
						continue;
					}
					fputc(character, fileF);
					continue;
				}
				fwrite(&hold[0], 1, hold.size(), fileF);
				fputc(character, fileF);
				hold.clear();
				count = 0;
				continue;
			}
			break;
		case CHECK_END:
			if (character == '-')
			{
                // printf("%c\n", character);
				// hold.clear(); //<==
				client.request.subState = CR_END;
                continue;
			}
            setError(client, "Error: Body::read state CHECK_END", 400);
			return;
		case CR_END:
			if (character == '\r')
			{
				// hold.clear();
				client.request.subState = LF_END;
                continue;
			}
            setError(client, "Error: Body::read state CR_END", 400);
			return;
		case LF_END: // check size of content length
			if (character == '\n')
			{
                if ((isEncodChunk(client) == false && countLength == client.request.contentLength)
                || (isEncodChunk(client) == true && client.request.getEncodChunkState() == LF_END_CHUNKED))
                {
                    if (fileF != NULL) {
                        fclose(fileF);
                        fileF = NULL;
                    }
                    client.request.setDeleteFiles(false);
				    client.request.setReqstDone(201);
                }
                else
                {
                    setError(client, "Error: Body::read LF_END countLength != client.request.contentLength", 400);
                }
				hold.clear();
				return;
			}
            setError(client, "Error: Body::read state LF_END", 400);
			return;
		}
		hold += character;
	}
}

void Body::writeBody(Client &client, string &buffer, ssize_t &size)
{
	fwrite(&buffer[0], 1, size, fileF);
	countLength += size;
	// printf("counlen = %ld len = %ld\n", client.request.contentLength, countLength);
	if ((isEncodChunk(client) == false && countLength == client.request.contentLength)
    || (isEncodChunk(client) == true && client.request.getEncodChunkState() == LF_END_CHUNKED))
	{
		client.request.setReqstDone(200);
		fclose(fileF);
        fileF = NULL;
        client.request.setDeleteFiles(false);
	}
	if (isEncodChunk(client) == false && countLength > client.request.contentLength)
	{
        setError(client, "countLength > client.request.ContentLength", 400);
		return;
	}
}

int Body::createFile(Client &client, const string &value, string &fileName)
{
	size_t fileNamePos = value.find("filename=");

	if (fileNamePos == string::npos)
		return setError(client, "Body::createFile filename", 400);
	fileName = trim(value.substr(fileNamePos + sizeof("filename")), " \"");
    fileName = getUploadPath(client) + fileName;
    printf("fileName = %s\n", &fileName[0]);//////


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
		return setError(client, "Body::createFile fail to open file", 500);
	return 0;
}

bool Body::RandomFile(Request &Request, const string& path, const string& extension)
{
	string randomName = "file-" + intToString(time(NULL));

    randomName = path + randomName + extension;

	ifstream inf(randomName.c_str());
    if (inf.is_open())
	{
		printf("error mktemp\n");
		return false;
	}
    printf("randomName = %s\n", &randomName[0]);
	fileF = fopen(randomName.c_str(), "w");
	if (!fileF)
	{
		printf("error Body::CGI fopen\n");
		return false;
	}
	File file;
	file.fileName = randomName;
	Request.files.push_back(file);
	return true;
}

int Body::setError(Client &client, const string& errorMsg, int statu)
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
