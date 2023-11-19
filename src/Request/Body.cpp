#include "../../include/Request/Body.hpp"
#include "../../include/Server/Client.hpp"

int Body::multiPart(Client &client, string &buffer, ssize_t &size)
{
	unsigned char holdChar;

	countLength += size;
	if (isEncodChunk(client) == false && countLength > client.request.contentLength)
		return statu(client, "Body length higher than content length", 400);
	for (string::iterator it = buffer.begin(); it != buffer.end(); it++)
	{
		holdChar = *it;
		switch (client.request.subState)
		{
		case FIRST_BOUNDARY:
			if (count == sizeBoundary + 1 && ("\r\n" + hold) == boundary + "\r")
			{
				if (holdChar == '\n')
				{
					hold.clear();
					count = 0;
					client.request.subState = HANDLER_KEY;
					continue;
				}
			}
			if (count > sizeBoundary + 1)
				return statu(client, "Invalid first boundary", 400);
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
			return statu(client, "Invalid boundary", 400);
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
			return statu(client, "Boundary must end with LF (Line Feed)", 400);
		case HANDLER_KEY:
			if (count >= MAX_KEY)
				return statu(client, "Multipart key too long", 400);
			if (holdChar == ':')
			{
				client.request.subState = HANDLER_VALUE;
				count = 0;
				key = strToLower(hold);
				hold.clear();
				continue;
			}
			else if (!isValidKey(holdChar))
				return statu(client, "Invalid multipart key", 400);
			count++;
			break;
		case HANDLER_VALUE:
			if (count >= MAX_VALUE)
				return statu(client, "Multipart value too long", 400);
			if (holdChar == '\r')
			{
				File file;
				if (key == "content-disposition")
				{
					int returnValue = createFile(client, hold, file.fileName);
					if (returnValue != 0)
						return returnValue;
				}
				file.Content.insert(make_pair(key, trim(hold, " \t")));
				client.request.files.insert(client.request.files.begin(), file);
				if (client.request.files.size() > MAX_HEADERS)
					return statu(client, "Too many headers in multipart", 400);
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
			return statu(client, "Multipart Header value must end with LF (Line Feed)", 400);
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
			return statu(client, "Invalid multipart headers syntax", 400);
		case START_DATA:
			if (holdChar == '\n')
			{
				hold.clear();
				if (fileF == NULL)
					return statu(client, "No file in multipart provided", 400);
				client.request.subState = WRITE_DATA;
				continue;
			}
			return statu(client, "Multipart Headers must end with LF (Line Feed)", 400);
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
							return statu(client, "System call function failed: fwrite", 500);
						it = buffer.begin() + (size - sizeBoundary - 1);
					}
					else
					{
						if (fwrite(&buffer[index], 1, posBoundary - index, fileF) != (posBoundary - index))
							return statu(client, "System call function failed: fwrite", 500);
						it = buffer.begin() + posBoundary - 1;
					}
					continue;
				}
				if (fputc(holdChar, fileF) != holdChar)
					return statu(client, "System call function failed: fputc", 500);
				continue;
			}
			size_t holdSize = hold.size();
			if (fwrite(&hold[0], 1, holdSize, fileF) != holdSize)
				return statu(client, "System call function failed: fwrite", 500);
			if (fputc(holdChar, fileF) != holdChar)
				return statu(client, "System call function failed: fputc", 500);
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
			return statu(client, "Boundary must end with -", 400);
		case CR_END:
			if (holdChar == '\r')
			{
				client.request.subState = LF_END;
				continue;
			}
			return statu(client, "Multipart Headers must end with CR (Carriage Return)", 400);
		case LF_END:
			if (holdChar == '\n')
			{
				if ((isEncodChunk(client) == false && countLength == client.request.contentLength) || (isEncodChunk(client) == true && client.request.getEncodChunkState() == END_LAST_HEXA))
				{
					if (fileF != NULL)
					{
						fclose(fileF);
						fileF = NULL;
					}
					client.request.setDeleteFiles(false);
					return statu(client, "", 201);
				}
				hold.clear();
				return statu(client, "Invalid Length in multipart", 400);
			}
			return statu(client, "Multipart Headers must end with LF (Line Feed)", 400);
		}
		hold += holdChar;
	}
	return statu(client, "", 0);
}

int Body::writeBody(Client &client, string &buffer, ssize_t &size)
{
	if (fwrite(&buffer[0], 1, size, fileF) != (size_t)size)
		return statu(client, "System call function failed: fwrite", 500);
	countLength += size;
	// printf("counlen = %ld len = %ld\n", client.request.contentLength, countLength);
	if ((isEncodChunk(client) == false && countLength == client.request.contentLength) || (isEncodChunk(client) == true && client.request.getEncodChunkState() == END_LAST_HEXA))
	{
		fclose(fileF);
		fileF = NULL;
		client.request.setDeleteFiles(false);
		if (client.response.isCgi)
			return statu(client, "", 200);
		return statu(client, "", 201);
	}
	if (isEncodChunk(client) == false && countLength > client.request.contentLength)
		return statu(client, "Invalid Body length", 400);
	return statu(client, "", 0);
}

int Body::skipBody(Client &client, ssize_t &size)
{
	countLength += size;
	printf("size = %ld\n", size);
	if ((isEncodChunk(client) == false && countLength == client.request.contentLength) || (isEncodChunk(client) == true && client.request.getEncodChunkState() == END_LAST_HEXA))
	{
		if (!client.response.method_allowd)
			return statu(client, "Method not allowed", 405);
		if (client.request.Method == "DELETE")
			return statu(client, "", deleteMthod(client));
		return statu(client, "", 200);
	}
	if (isEncodChunk(client) == false && countLength > client.request.contentLength)
		return statu(client, "Invalid Body length", 400);
	return statu(client, "", 0);
}

int Body::createFile(Client &client, const string &value, string &fileName)
{
	size_t fileNamePos = value.find("filename=");
	// printf("fileName = |%s|\n", &fileName[0]);//////
	if (fileNamePos == string::npos)
		return statu(client, "No file name provided", 400);
	fileName = trim(value.substr(fileNamePos + sizeof("filename")), " \"");
	if (fileName.empty())
		return statu(client, "No file name provided", 400);
	fileName = getUploadPath(client) + fileName;
	if (access(fileName.c_str(), F_OK) == 0)
		return statu(client, "File already exists", 400);
	;
	fileF = fopen(fileName.c_str(), "w");
	if (!fileF)
		return statu(client, "Generate file failed for Multipart", 500);
	return 0;
}

bool Body::RandomFile(Request &Request, const string &path, const string &extension)
{
	string randomName;

	while (true)
	{
		randomName = "file-" + intToString(time(NULL));
		randomName = path + randomName + extension;
		// printf("randomName = %s\n", &randomName[0]);
		ifstream inf(randomName.c_str());
		if (inf.is_open() == false)
			break;
	}
	fileF = fopen(randomName.c_str(), "w");
	if (!fileF)
		return false;
	File file;
	file.fileName = randomName;
	Request.files.push_back(file);
	return true;
}

int Body::statu(Client &client, const string &errorMsg, int statu)
{
	client.request.setReqstDone(statu);
	client.request.setErrorMsg(errorMsg);
	return statu;
}

bool Body::isEncodChunk(Client &client)
{
	return client.request.getDecodeFlag();
}

void Body::setBoundary(const string &boundary)
{
	this->boundary = boundary;
}

string Body::getBoundary(void) const
{
	return boundary;
}

void Body::setSizeBoundary(const int &sizeBoundary)
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
	if (fileF != NULL)
	{
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
