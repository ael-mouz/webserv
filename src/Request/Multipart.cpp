#include "../../include/Request/Multipart.hpp"
#include "../../include/Request/Request_FSM.hpp"

void Multipart::read(Request_Fsm &Request, string &buffer, ssize_t &size)
{
	unsigned char character;

	// countLength += size;
	for (string::iterator it = buffer.begin(); it != buffer.end(); it++)
	{
		character = *it;
		// countLength += 1;
		switch (Request.subState)
		{
		case START_BOUND:
			if (count < Request.sizeBoundary && character == Request.boundary[count])
			{
				count++;
				break;
			}
			if (count == Request.sizeBoundary && ("\r\n" + Request.hold) == Request.boundary)
			{
				Request.hold.clear();
				count = 0;
				Request.subState = AFTER_BOUND;
			}
			else if (count > Request.sizeBoundary)
			{
				printf("Error: Multipart::read state START_BOUND i = %ld c = %c\n", it - buffer.begin(), character);
				Request.mainState = 0;
				return;
			}
		case AFTER_BOUND:
			if (character == '-')
			{
				Request.subState = CHECK_END;
				break;
			}
			else if (character == '\r')
			{
				Request.subState = LF_BOUNDRY;
				Request.hold.clear();
				writeToFile = 0;
				continue;
			}
			else
			{
				printf("Error: Multipart::read state AFTER_BOUND i = %ld c = %d\n", it - buffer.begin(), character);
				Request.mainState = 0;
				return;
			}
		case LF_BOUNDRY:
			if (character == '\n')
			{
				if (fileF != NULL)
				{
					fclose(fileF);
					fileF = NULL;
				}
				Request.subState = HANDLER_KEY;
				continue;
			}
			else
			{
				printf("Error: Headers::read state BEFOR_KRY i = %ld c = %c\n", it - buffer.begin(), character);
				Request.mainState = 0;
				return;
			}
		case HANDLER_KEY: // set max
			if (character == ':')
			{
				Request.subState = HANDLER_VALUE;
				count = 0;
				Request.key = Request.hold;
				Request.hold.clear(); // store key
				continue;
			}
			else if (!ValidKey(character))
			{
				printf("Error: Multipart::read state HANDLER_KEY i = %ld c = %c\n", it - buffer.begin(), character);
				Request.mainState = 0;
				return;
			}
			else if (count >= MAX_KEY)
			{
				printf("Error: Headers::read state KEY MAX_KEY i = %d c = %c\n", count, character);
				Request.mainState = 0;
				return;
			}
			count++;
			break;
		case HANDLER_VALUE: // set max
			if (character == '\r')
			{
				File file;
				if (Request.key == "Content-Disposition")
				{
					createFile(Request.hold, file.fileName); // check the error in other stat // organize file
					if (writeToFile == -1)
					{
						printf("Error: Multipart::read state HANDLER_VALUE fdFile i = %ld c = %c\n", it - buffer.begin(), character);
						Request.mainState = 0;
						return;
					}
				}
				file.Content.insert(make_pair(Request.key, trim(Request.hold, " \t")));
				Request.files.insert(Request.files.begin(), file); // clear file ??
				Request.hold.clear();
				Request.key.clear();
				Request.subState = LF_VALUE;
				count = 0;
				continue;
			}
			if (count >= MAX_VALUE)
			{
				printf("Error: Headers::read state VALUE MAX_VALUE i = %d c = %c\n", count, character);
				Request.mainState = 0;
				return;
			}
			count++;
			break;
		case LF_VALUE:
			if (character == '\n')
			{
				Request.subState = CHECK_AFTER_VALUE;
				Request.hold.clear();
				continue;
			}
			else
			{
				printf("Error: Headers::read state AFTER_VALUE i = %ld c = %c\n", it - buffer.begin(), character);
				Request.mainState = 0;
				return;
			}
		case CHECK_AFTER_VALUE:
			if (character == '\r')
			{
				Request.subState = START_DATA;
				continue;
			}
			else if (ValidKey(character))
			{
				Request.subState = HANDLER_KEY;
			}
			else
			{
				printf("Error: Headers::read state CHECK i = %ld c = %c\n", it - buffer.begin(), character);
				Request.mainState = 0;
				return;
			}
			break;
		case START_DATA:
			if (character == '\n')
			{
				Request.hold.clear();
				Request.subState = WRITE_DATA;
				continue;
			}
			else
			{
				printf("Error: Headers::read state AFTER_WRITE_DATA i = %ld c = %c\n", it - buffer.begin(), character);
				Request.mainState = 0;
				return;
			}
			break;
		case WRITE_DATA:
			if (count < Request.sizeBoundary && character == Request.boundary[count])
			{
				count++;
				if (count == Request.sizeBoundary)
					Request.subState = AFTER_BOUND;
			}
			else
			{
				if (count == 0)
				{
					if (buffer.end() - it > Request.sizeBoundary + 4)
					{
						int index = it - buffer.begin();
						size_t posBoundary = buffer.find(Request.boundary, index);
						if (posBoundary == string::npos)
						{
							fwrite(&buffer[index], 1, size - Request.sizeBoundary - index, fileF);
							countLength += size - Request.sizeBoundary - index;
							it = buffer.begin() + (size - Request.sizeBoundary - 1);
						}
						else
						{
							fwrite(&buffer[index], 1, posBoundary - index, fileF);
							countLength += posBoundary - index;
							it = buffer.begin() + posBoundary - 1;
						}
						continue;
					}
					fputc(character, fileF);
					countLength += 1;
					continue;
				}
				fwrite(&Request.hold[0], 1, Request.hold.size(), fileF);
				countLength += Request.hold.size();
				fputc(character, fileF);
				countLength += 1;
				Request.hold.clear();
				count = 0;
				continue;
			}
			break;
		case CHECK_END:
			if (character == '-')
			{
				Request.hold.clear(); //<==
				(fileF != NULL) && (fclose(fileF));
				Request.subState = CR_END;
			}
			else
			{
				printf("Error: Headers::read state CHECK_END i = %ld c = %c\n", it - buffer.begin(), character);
				Request.mainState = 0;
				return;
			}
			break;
		case CR_END:
			if (character == '\r')
			{
				Request.hold.clear();
				Request.subState = LF_END;
			}
			else
			{
				printf("Error: Headers::read state CR_END i = %ld c = %c\n", it - buffer.begin(), character);
				Request.mainState = 0;
				return;
			}
			break;
		case LF_END: // check size of content length
			if (character == '\n')
			{
				Request.ReqstDone = 1;
				Request.hold.clear();
				return;
			}
			else
			{
				printf("Error: Headers::read state CR_END i = %ld c = %c\n", it - buffer.begin(), character);
				Request.mainState = 0;
				return;
			}
			break;
		}
		Request.hold += character;
		// countLength += 1;
		// printf("i = %ld stat = %d c= %d : %c\n",it-buffer.begin(),Request.state, c, c);
	}
}

void Multipart::CGI(Request_Fsm &Request, string &buffer, ssize_t &size)
{
	if (writeToFile == 0)
	{
		if (createFileCGI(Request) == false)
		{
			Request.mainState = 0;
			return;
		}
	}
	// printf("%s\nsize = %ld suzwMeth = %ld\n", &buffer[0], size, buffer.size());
	fwrite(&buffer[0], 1, size, fileF);
	countLength += size;
	// printf("counlen = %ld len = %ld\n", Request.ContentLength, countLength);
	if (countLength == Request.ContentLength)
	{
		writeToFile = 0;
		Request.ReqstDone = 1;
		fclose(fileF);
	}
	if (countLength > Request.ContentLength)
	{
		// printf("countLength > Request.ContentLength\n"); Request.mainState = 0;
		return;
	}
}
void Multipart::createFile(const string &value, string &fileName)
{
	size_t fileNamePos = value.find("filename=");

	if (fileNamePos == string::npos)
	{
		writeToFile = 0;
		return;
	}
	fileName = trim(value.substr(fileNamePos + sizeof("filename")), " \"");
	fileName = "/goinfre/yettabaa/untitled folder/" + fileName;
	fileF = fopen(&fileName[0], "w");
	if (!fileF)
	{
		writeToFile = -1;
		return;
	}
	writeToFile = 1;
	// // std::cout << fileName << "\n";
}

bool Multipart::createFileCGI(Request_Fsm &Request)
{
	char *randomName = strdup("file-XXXXXXXXXXXXXXXXX");

	if (mktemp(randomName) == NULL)
	{
		printf("error mktemp\n");
		delete randomName;
		return false;
	}
	string fileName = randomName;
	delete randomName;
	fileName = "/goinfre/yettabaa/untitled folder/" + fileName;
	fileF = fopen(&fileName[0], "w");
	if (!fileF)
	{
		printf("error Multipart::CGI fopen\n");
		return false;
	}
	File file;
	file.fileName = fileName;
	Request.files.push_back(file);
	writeToFile = 1;
	return true;
}

Multipart::Multipart()
{
	count = 2;
	countLength = 0;
	writeToFile = 0;
	fileF = NULL;
	// out = NULL;
}

Multipart::~Multipart() {}
