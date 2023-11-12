#include "../../include/Request/Body.hpp"
#include "../../include/Server/Client.hpp"

void Body::multiPart(Client &client, string &buffer, ssize_t &size)
{
	unsigned char character;

	countLength += size;
    // printf("parsheadelen = %ld countLength = %ld\n",client.request.ContentLength, countLength);
    if (client.request.decodeFlag == false && countLength > client.request.ContentLength)
    {
        printf("Error: Body::read size > client.request.ContentLength\n");
		client.request.ReqstDone = 400;
		return;
    }
	for (string::iterator it = buffer.begin(); it != buffer.end(); it++)
	{
		character = *it;
		switch (client.request.subState)
		{
		case FIRST_BOUNDARY:
			if (count == client.request.sizeBoundary +1 && ("\r\n" + client.request.hold) == client.request.boundary + "\r")
			{
                if (character == '\n')
                {
				    client.request.hold.clear();
				    count = 0;
				    client.request.subState = HANDLER_KEY;
                    continue;
                }
			}
			if (count > client.request.sizeBoundary +1)
			{
				printf("Error: Body::read state START_BOUND i = %ld c = %c\n", it - buffer.begin(), character);
				client.request.ReqstDone = 400;
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
				client.request.hold.clear();
				// writeToFile = 0;
                continue;
			}
			else
			{
				printf("Error: Body::read state AFTER_BOUND i = %ld c = %d\n", it - buffer.begin(), character);
				client.request.ReqstDone = 400;
				return;
			}
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
			else
			{
				printf("Error: Headers::read state BEFOR_KRY i = %ld c = %c\n", it - buffer.begin(), character);
				client.request.ReqstDone = 400;
				return;
			}
		case HANDLER_KEY: // set max
			if (character == ':')
			{
				client.request.subState = HANDLER_VALUE;
				count = 0;
				client.request.key = client.request.hold;
				client.request.hold.clear(); // store key
                continue;
			}
			else if (!ValidKey(character))
			{
				printf("Error: Body::read state HANDLER_KEY i = %ld c = %c\n", it - buffer.begin(), character);
				client.request.ReqstDone = 400;
				return;
			}
			else if (count >= MAX_KEY)
			{
				printf("Error: Headers::read state KEY MAX_KEY i = %d c = %c\n", count, character);
				client.request.ReqstDone = 400;
				return;
			}
			count++;
			break;
		case HANDLER_VALUE: // set max
			if (character == '\r')
			{
				File file;
				if (client.request.key == "Content-Disposition")
				{
					if (createFile(client ,client.request.hold, file.fileName) == false)
					{
						printf("Error: Body::read state HANDLER_VALUE ""fdFile i = %ld ""c = %c\n",it - buffer.begin(), character);
						client.request.ReqstDone = 500;
						return;
					}
                    // file.toDelete = true;
				}
				file.Content.insert(make_pair(client.request.key, trim(client.request.hold, " \t")));
				client.request.files.insert(client.request.files.begin(), file); // clear file ??
				client.request.hold.clear();
				client.request.key.clear();
				client.request.subState = LF_VALUE;
				count = 0;
				continue;
			}
			if (count >= MAX_VALUE)
			{
				printf("Error: Headers::read state VALUE MAX_VALUE i = %d c = ""%c\n", count, character);
				client.request.ReqstDone = 400;
				return;
			}
			count++;
			break;
		case LF_VALUE:
			if (character == '\n')
			{
				client.request.subState = CHECK_AFTER_VALUE;
				client.request.hold.clear();
                continue;
			}
			else
			{
				printf("Error: Headers::read state AFTER_VALUE i = %ld c = %c\n", it - buffer.begin(), character);
				client.request.ReqstDone = 400;
				return;
			}
		case CHECK_AFTER_VALUE:
			if (character == '\r')
			{
				client.request.subState = START_DATA;
                continue;
			}
			else if (ValidKey(character))
			{
				client.request.subState = HANDLER_KEY;
			}
			else
			{
				printf("Error: Headers::read state CHECK i = %ld c = %c\n", it - buffer.begin(), character);
				client.request.ReqstDone = 400;
				return;
			}
			break;
		case START_DATA:
			if (character == '\n')
			{
				client.request.hold.clear();
				client.request.subState = WRITE_DATA;
                continue;
			}
			else
			{
				printf("Error: Headers::read state AFTER_WRITE_DATA i = %ld c ""= %c\n", it - buffer.begin(), character);
				client.request.ReqstDone = 400;
				return;
			}
			break;
		case WRITE_DATA:
			if (count < client.request.sizeBoundary && character == client.request.boundary[count])
			{
				count++;
				if (count == client.request.sizeBoundary)
					client.request.subState = AFTER_BOUNDARY;
			}
			else
			{
				if (count == 0)
				{
					if (buffer.end() - it > client.request.sizeBoundary + 4)
					{
						int index = it - buffer.begin();
						size_t posBoundary = buffer.find(client.request.boundary, index);
						if (posBoundary == string::npos)
						{
							fwrite(&buffer[index], 1, size - client.request.sizeBoundary - index, fileF);
							it = buffer.begin() + (size - client.request.sizeBoundary - 1);
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
				fwrite(&client.request.hold[0], 1, client.request.hold.size(), fileF);
				fputc(character, fileF);
				client.request.hold.clear();
				count = 0;
				continue;
			}
			break;
		case CHECK_END:
			if (character == '-')
			{
				client.request.hold.clear(); //<==
				client.request.subState = CR_END;
			}
			else
			{
				printf("Error: Headers::read state CHECK_END i = %ld c = %c\n", it - buffer.begin(), character);
				client.request.ReqstDone = 400;
				return;
			}
			break;
		case CR_END:
			if (character == '\r')
			{
				client.request.hold.clear();
				client.request.subState = LF_END;
			}
			else
			{
				printf("Error: Headers::read state CR_END i = %ld c = %c\n", it - buffer.begin(), character);
				client.request.ReqstDone = 400;
				return;
			}
			break;
		case LF_END: // check size of content length
			if (character == '\n')
			{
                // printf("parsheadelen = %ld countLength = %ld\n",client.request.ContentLength, countLength);
                if (client.request.decodeFlag == false && countLength != client.request.ContentLength)
                {
                    printf("Error: Body::read LF_END countLength != client.request.ContentLength\n");
	            	client.request.ReqstDone = 400;
	            	return;
                }
                if (fileF != NULL) {
                    fclose(fileF);
                }
                client.request.deleteFiles = false;
				client.request.ReqstDone = 201;
				client.request.hold.clear();
				return;
			}
			else
			{
				printf("Error: Headers::read state CR_END i = %ld c = %c\n", it - buffer.begin(), character);
				client.request.ReqstDone = 400;
				return;
			}
			break;
		}
		client.request.hold += character;
	}
}

void Body::mimeType(Client &client, string &buffer, ssize_t &size)
{
	fwrite(&buffer[0], 1, size, fileF);
	countLength += size;
	// printf("counlen = %ld len = %ld\n", client.request.ContentLength, countLength);
	if (client.request.decodeFlag == false && countLength == client.request.ContentLength)
	{
		writeToFile = 0;
		client.request.ReqstDone = 200;
		fclose(fileF);
        fileF = NULL;
        client.request.deleteFiles = false;
	}
	if (countLength > client.request.ContentLength)
	{
		// printf("countLength > client.request.ContentLength\n"); client.request.mainState =// 0;
        client.request.ReqstDone = 400;
		return;
	}
}

void Body::CGI(Client &client, string &buffer, ssize_t &size)
{
	if (writeToFile == 0) // creat it in headers like mime type
	{
		if (RandomFile(client.request, "/tmp/.", "") == false)
		{
			client.request.ReqstDone = 500;
			return;
		}
	}
	// printf("%s\nsize = %ld suzwMeth = %ld\n", &buffer[0], size,
	// buffer.size());
	fwrite(&buffer[0], 1, size, fileF);
	countLength += size;
	// printf("counlen = %ld len = %ld\n", client.request.ContentLength, countLength);
	if (client.request.decodeFlag == false && countLength == client.request.ContentLength)
	{
		writeToFile = 0;
		client.request.ReqstDone = 200;
		fclose(fileF);
        fileF = NULL;
        client.request.deleteFiles = false;
	}
	if (countLength > client.request.ContentLength)
	{
		// printf("countLength > client.request.ContentLength\n"); client.request.mainState =// 0;
        client.request.ReqstDone = 400;
		return;
	}
}
bool Body::createFile(Client &client, const string &value, string &fileName)
{
	size_t fileNamePos = value.find("filename=");

	if (fileNamePos == string::npos)
		return false;
	fileName = trim(value.substr(fileNamePos + sizeof("filename")), " \"");
    fileName = getUploadPath(client) + fileName;
    // std::string path = client.response.route.UploadPath; //function
    // (path != "default") ? fileName = path + fileName : fileName = client.response.Config->GlobalUpload + fileName;
    // printf("fileName = %s\n", &fileName[0]);//////
	fileF = fopen(&fileName[0], "w");
	if (!fileF)
		return false;
	return true;
}

bool Body::RandomFile(Request &Request, const string& path, const string& extension)
{
	// string randomName = "/tmp/.file-XXXXXXXXXXXXXXXXX";
	string randomName = "file-" + intToString(time(NULL));

    randomName = path + randomName + extension;

	ifstream inf(randomName.c_str());
    printf("randomName = %s\n", &randomName[0]);
    if (inf.is_open())
	{
		printf("error mktemp\n");
		return false;
	}
    // inf.close();
    // close(fd);
	fileF = fopen(randomName.c_str(), "w");
	if (!fileF)
	{
		printf("error Body::CGI fopen\n");
		return false;
	}
	File file;
	file.fileName = randomName;
	Request.files.push_back(file);
	writeToFile = 1;
	return true;
}

void Body::reset()
{
	count = 2;
	countLength = 0;
	writeToFile = 0;
    if (fileF != NULL) {
        fclose(fileF);
    }
	fileF = NULL;
}

Body::Body()
{
	count = 2;
	countLength = 0;
	writeToFile = 0;
	fileF = NULL;
	// out = NULL;
}

Body::~Body() {}
