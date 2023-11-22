#include "../../include/Response/Response.hpp"
#include "../../include/Config/ServerConfig.hpp"
#include "../../include/Server/Client.hpp"

void Response::sendResponse(Client &client)
{
	responseString.clear();
	if (responseDone && !HeaderResponse.empty() && !isBodySent)
	{
		if (!isHeaderSent)
		{
			responseString = HeaderResponse;
			isHeaderSent = true;
			std::string status, StringStatus;
			if (responseStatus >= "400")
				status = "[" FG_RED + responseStatus + RESET_ALL + "]",
				StringStatus = "[" FG_RED + this->Config->status.getStatus(responseStatus) + RESET_ALL + "]";
			else
				status = "[" FG_GREEN + responseStatus + RESET_ALL + "]",
				StringStatus = "[" FG_GREEN + this->Config->status.getStatus(responseStatus) + RESET_ALL + "]";
			logMessage(SRES, client.clientHost, client.socketClient, status + " " + StringStatus + " Response sent to " + client.clientIP);
			if (client.request.Method == "HEAD")
				head_method = true;
		}
		if (!BodyResponse.empty() && !head_method)
			responseString += BodyResponse,
				responseSent = isBodySent = true;
		else if (fptr && !feof(fptr) && !head_method)
		{
			char buffer[1024];
			ssize_t bytesRead = fread(buffer, 1, sizeof(buffer), fptr);
			if (bytesRead > 0)
				responseString += std::string(buffer, bytesRead);
			else
				responseSent = isBodySent = true;
		}
		else
			responseSent = isBodySent = true;
		if (responseString.length() > 0)
		{
			if (send(client.socketClient, responseString.c_str(), responseString.length(), 0) <= 0)
				closeClient = responseSent = true;
		}
	}
	if (responseSent)
		if (client.request.mapHeaders.find("connection") != client.request.mapHeaders.end())
			if (client.request.mapHeaders.find("connection")->second == "close")
				closeClient = true;
}

void Response::startResponse(Client &client)
{
	getConfig(client);
	parseUri(client.request.URI);
	getRoute();
	getFULLpath();
	genrateRederiction(client);
	checkerPath(client);
	regenerateExtonsion();
	checkAcceptedMethod(client);
}

void Response::CGI(Client &client)
{
	mergeHeadersValuesCGI(client);
	generateEnvCGI(client);
	handleScriptCGI(client);
}

void Response::handleNormalFiles(Client &client)
{
	if (this->responseDone)
		return;
	this->fptr = fopen(this->fullpath.c_str(), "rb");
	if (!this->fptr)
		return (generateResponse("500"));
	fseek(this->fptr, 0, SEEK_END);
	this->fileSize = ftell(this->fptr);
	fseek(this->fptr, 0, SEEK_SET);
	std::stringstream header;
	std::multimap<std::string, std::string>::iterator it = client.request.mapHeaders.find("range");
	if (it != client.request.mapHeaders.end())
		handleRange(header, it->second);
	else
	{
		this->responseStatus = "200";
		std::string status = "200";
		header << "HTTP/1.1 " << status << " " << this->Config->status.getStatus("200") << "\r\n";
		header << "Accept-Ranges: " << std::string("bytes") << "\r\n";
		header << "Content-Type: " << this->Config->mime.getMimeType(this->extension) << "\r\n";
		header << "Content-Length: " << fileSize << "\r\n";
		header << "\r\n";
	}
	this->HeaderResponse = header.str();
	this->responseDone = true;
}

void Response::handleRange(stringstream &header, const std::string &range)
{
	size_t equalsPos = range.find('=');
	size_t hyphenPos = range.find('-');
	if (equalsPos == std::string::npos || hyphenPos == std::string::npos)
		return (generateResponse("416"));
	this->offset = 0;
	std::string startByteStr = range.substr(equalsPos + 1, hyphenPos - equalsPos - 1);
	std::istringstream(startByteStr) >> this->offset;
	if (this->offset >= this->fileSize)
		return (generateResponse("416"));
	fseek(this->fptr, this->offset, SEEK_SET);
	this->responseStatus = "206";
	std::string status = "206";
	header << "HTTP/1.1 " << status << " " << this->Config->status.getStatus("206") << "\r\n";
	header << "Accept-Ranges: " << std::string("bytes") << "\r\n";
	header << "Content-Type: " << this->Config->mime.getMimeType(this->extension) << "\r\n";
	header << "Content-Range: " << std::string("bytes ") << this->offset << "-" << (this->fileSize - 1) << "/" << this->fileSize << "\r\n";
	this->fileSize -= this->offset;
	header << "Content-Length: " << fileSize << "\r\n";
	header << "\r\n";
}

void Response::checkAcceptedMethod(Client &client)
{
	if (this->route.Accepted_Methods == "on")
	{
		if (client.request.Method == this->route.Accepted_Methods_ ||
			client.request.Method == this->route.Accepted_Methods__ ||
			client.request.Method == this->route.Accepted_Methods___)
			method_allowd = true;
	}
}

void Response::checkErrorsRequest(Client &client)
{
	if (client.request.ReqstDone == 200 && client.request.Method == "DELETE")
	{
		std::stringstream headers;
		this->responseStatus = "204";
		std::string status = "204";
		headers << "HTTP/1.1 " << status << " " << this->Config->status.getStatus("204") << "\r\n";
		headers << "Content-Type: " << this->Config->mime.getMimeType("html") << "\r\n";
		headers << "Content-Length: " << 0 << "\r\n";
		headers << "\r\n";
		this->HeaderResponse = headers.str();
		this->responseDone = true;
		return;
	}
	if (client.request.ReqstDone == 201)
	{
		std::stringstream headers, body_, files;
		vector<File>::iterator it = client.request.files.begin();
		vector<File>::iterator end = client.request.files.end();
		for (; it != end; it++)
			files << "<li>" + it->fileName + " " + (it->fileExists ? " : <strong>Not Created (File already exists)</strong>" : ": <strong>Created</strong>") + "</li>\n ";
		body_ << GENERATE_UPLOAD_HTML(files.str());
		this->responseStatus = "201";
		std::string status = "201";
		headers << "HTTP/1.1 " << status << " " << this->Config->status.getStatus("201") + "\r\n";
		headers << "Content-Type: " << this->Config->mime.getMimeType("html") << "\r\n";
		headers << "Content-Length: " << body_.str().length() << "\r\n";
		headers << "\r\n";
		this->HeaderResponse = headers.str();
		this->BodyResponse = body_.str();
		this->responseDone = true;
		return;
	}
	else if (client.request.ReqstDone != 200)
		return (generateResponse(intToString(client.request.ReqstDone)));
}

void Response::getConfig(Client &client)
{
	this->Config = &client.serverConf.DefaultServerConfig;
	std::multimap<std::string, std::string>::iterator it = client.request.mapHeaders.find("host");
	std::string servername;
	if (it != client.request.mapHeaders.end())
		servername = trim(it->second, " \t");
	if (!servername.empty())
	{
		std::map<std::string, ServerConfig>::iterator it;
		it = client.serverConf.OtherServerConfig.find(servername);
		if (it != client.serverConf.OtherServerConfig.end())
			this->Config = &it->second;
	}
}

void Response::regenerateExtonsion()
{
	if (this->responseDone)
		return;
	size_t pos5 = this->fullpath.find_last_of(".");
	if (pos5 != std::string::npos)
		this->extension = this->fullpath.substr(pos5 + 1, this->fullpath.length() - pos5 + 1);
	if (this->extension == "pl" || this->extension == "py" ||
		this->extension == "php" || this->extension == "rb")
		this->isCgi = true;
}

void Response::parseUri(std::string uri)
{
	size_t posquery = uri.find("?");
	if (posquery != std::string::npos)
		this->query = uri.substr(posquery + 1, uri.length() - posquery),
		uri = uri.substr(0, posquery);
	std::vector<std::string> parts = splitString(uri, "/");
	std::vector<std::string> resolvedPath;
	for (std::vector<std::string>::const_iterator it = parts.begin(); it != parts.end(); it++)
	{
		if (*it != ".." && *it != "." && !it->empty())
			resolvedPath.push_back(*it);
		else if (!resolvedPath.empty() && *it != "." && !it->empty())
			resolvedPath.pop_back();
	}
	std::string path;
	for (std::vector<std::string>::const_iterator it2 = resolvedPath.begin(); it2 != resolvedPath.end(); it2++)
		path += '/' + *it2;
	if (uri[uri.length() - 1] == '/')
		path += "/";
	if (!path.empty())
		uri = path;
	else if (path.empty())
		uri = "/";
	size_t pos = uri.find_last_of(".");
	if (pos != std::string::npos)
	{
		size_t pos1 = uri.find("/", pos);
		if (pos1 != std::string::npos)
			this->path_info = uri.substr(pos1, uri.length() - pos1),
			this->fullpath = uri.substr(0, pos1);
		else
			this->fullpath = uri;
	}
	else
		this->fullpath = uri;
	if (this->path_info.empty())
		this->path_info = "/";
	size_t pos1 = this->fullpath.find_last_of(".");
	if (pos1 != std::string::npos)
		this->extension = this->fullpath.substr(pos1 + 1, this->fullpath.length() - pos1 + 1);
}

void Response::getRoute()
{
	std::string dir;
	this->route = this->Config->getRoute(this->fullpath);
	if (this->route.RoutePath == "default")
	{
		if (!this->extension.empty())
			this->route = this->Config->getRoute(this->extension);
		if (this->route.RoutePath == "default")
		{
			dir = getParentDirectories(this->fullpath);
			while (this->route.RoutePath == "default" && !dir.empty() && dir != "/")
			{
				this->route = this->Config->getRoute(dir);
				dir = getParentDirectories(dir);
			}
			if (dir == "/" && this->route.RoutePath == "default")
				this->route = this->Config->getRoute(dir), match = 0;
			else
				match = 1;
		}
		else
			match = 2;
	}
	else
		match = 3;
}
void Response::genrateRederiction(Client &client)
{
	if (this->responseDone)
		return;
	if (this->route.Redirection == "on")
	{
		std::stringstream Headers__;
		Headers__ << "HTTP/1.1 " + this->route.RedirectionStatus + " " + this->Config->status.getStatus(this->route.RedirectionStatus) + " \r\n";
		Headers__ << "Location: " + this->route.RedirectionURL + "\r\n";
		Headers__ << "\r\n";
		this->HeaderResponse = Headers__.str();
		this->responseStatus = this->route.RedirectionStatus;
		this->responseDone = true;
		return;
	}
	if (this->fullpath[this->fullpath.length() - 1] == '/')
		this->fullpath.erase(this->fullpath.end() - 1);
	else if (this->extension.empty() && isDirectory(this->fullpath.c_str()) == 1 &&
			 (client.request.Method == "GET" || client.request.Method == "POST" || client.request.Method == "DELETE"))
	{
		std::stringstream Headers__;
		std::string status = "302";
		Headers__ << "HTTP/1.1 " << status << " " << this->Config->status.getStatus("302") << "\r\n";
		Headers__ << "Location: " << std::string("http://") + this->Config->Host + ":" + this->Config->Port + this->entryPath + std::string("/") << "\r\n";
		Headers__ << "\r\n";
		this->HeaderResponse = Headers__.str();
		// std::cout << convertText(this->HeaderResponse) << std::endl;
		this->responseStatus = status;
		this->responseDone = true;
		return;
	}
}
void Response::checkerPath(Client &clinet)
{
	if (this->responseDone)
		return;
	int dirr = isDirectory(this->fullpath.c_str());
	if (dirr == 1)
	{
		if (clinet.request.Method == "DELETE")
			return (this->responseDone = true, void());
		if (this->route.Autoindex == "on")
			return (generateAutoIndex());
		if (this->route.Index == "default")
			this->fullpath += "/index.html";
		else
			this->fullpath = this->fullpath + "/" + this->route.Index;
		dirr = isDirectory(this->fullpath.c_str());
		if (dirr == -1 || dirr == 1)
			return (generateResponse("404"));
	}
	else if (dirr == -1)
		return (generateResponse("404"));
}

void Response::getFULLpath()
{
	if (this->responseDone)
		return;
	std::string tmp = route.RoutePath;
	if (tmp[tmp.length() - 1] == '/')
		tmp.erase(tmp.end() - 1);
	std::string newtmp;
	if (route.Root != "default" && route.RoutePath != "default" && (this->match == 1 || this->match == 3) && this->extension != tmp )
		this->entryPath = this->fullpath,
		newtmp = this->fullpath.substr(tmp.length(), this->fullpath.length() - tmp.length()),
		this->fullpath = this->route.Root + newtmp,
		this->path_translated = this->route.Root + this->path_info,
		this->root = this->route.Root;
	else if (route.Root == "default" && route.RoutePath != "default" && (this->match == 1 || this->match == 3) && this->extension != tmp)
		this->entryPath = this->fullpath,
		newtmp = this->fullpath.substr(tmp.length(), this->fullpath.length() - tmp.length()),
		this->fullpath = this->Config->GlobalRoot + newtmp,
		this->path_translated = this->Config->GlobalRoot + this->path_info,
		this->root = this->Config->GlobalRoot;
	else if (route.Root != "default" && route.RoutePath != "default")
		this->entryPath = this->fullpath,
		this->fullpath = this->route.Root + this->fullpath,
		this->path_translated = this->route.Root + this->path_info,
		this->root = this->route.Root;
	else
		this->entryPath = this->fullpath,
		this->fullpath = this->Config->GlobalRoot + this->fullpath,
		this->path_translated = this->Config->GlobalRoot + this->path_info,
		this->root = this->Config->GlobalRoot;
	// std::cout << "debug " << entryPath << std::endl;
	// std::cout << "debug " << fullpath << std::endl;
	// std::cout << "debug " << path_translated << std::endl;
	// std::cout << "debug " << root << std::endl;
}

void Response::generateAutoIndex(void)
{
	DIR *dir;
	struct dirent *entry;
	dir = opendir(this->fullpath.c_str());
	if (!dir)
		return (generateResponse("500"));
	std::stringstream autoIndex, autoIndexBody;
	while ((entry = readdir(dir)) != NULL)
	{
		if (entry->d_type == DT_DIR && std::strcmp(entry->d_name, ".") == 0)
			continue;
		std::string fileType = (entry->d_type == DT_DIR) ? "Directory" : "File";
		std::string icon = (entry->d_type == DT_DIR) ? "/images/folder.png" : "/images/file.png";
		std::string link;
		if (!this->entryPath.empty() && this->entryPath != "/" && this->entryPath[this->entryPath.length() - 1] != '/')
			link = this->entryPath + "/" + entry->d_name;
		else if (!this->entryPath.empty() && this->entryPath != "/")
			link = this->entryPath + entry->d_name;
		else
			link = entry->d_name;
		struct stat fileStat;
		std::string path;
		if (this->fullpath[this->fullpath.length() - 1] != '/')
			path = this->fullpath + "/" + entry->d_name;
		else
			path = this->fullpath + entry->d_name;
		autoIndexBody << "<tr>\n";
		autoIndexBody << "<td class='" << fileType << "'>\n";
		autoIndexBody << "<img src='" << icon << "' class='icon'></img>\n";
		autoIndexBody << "<a href='" << link << "' class='" << fileType << "'>" << entry->d_name << "</a>\n";
		autoIndexBody << "</td>\n";
		autoIndexBody << "<td class='" << fileType << " col'>" << fileType << "</td>\n";
		if (stat(path.c_str(), &fileStat) == 0)
		{
			std::string sizeStr = humanReadableSize(fileStat.st_size);
			autoIndexBody << "<td class='" << fileType << " col'>" << sizeStr << "</td>\n";
			autoIndexBody << "<td class='" << fileType << " col'>" << std::oct << (fileStat.st_mode & 0777) << "</td>\n";
			autoIndexBody << "<td class='" << fileType << " col'>" << std::ctime(&fileStat.st_atime) << "</td>\n";
			autoIndexBody << "<td class='" << fileType << " col'>" << std::ctime(&fileStat.st_mtime) << "</td>\n";
			autoIndexBody << "<td class='" << fileType << " col'>" << std::ctime(&fileStat.st_ctime) << "</td>\n";
		}
		autoIndexBody << "</tr>\n";
	}
	autoIndex << GENERATE_AUTOINDEX_HTML(this->fullpath, autoIndexBody.str());
	this->BodyResponse = autoIndex.str();
	std::stringstream header_;
	this->responseStatus = "200";
	std::string status = "200";
	header_ << "HTTP/1.1 " << status << " " << this->Config->status.getStatus("200") << "\r\n";
	header_ << "Content-Type: " << this->Config->mime.getMimeType("html") << "\r\n";
	header_ << "Content-length: " + intToString(autoIndex.str().length()) + "\r\n";
	header_ << "\r\n";
	this->HeaderResponse = header_.str();
	this->responseDone = true;
	closedir(dir);
	return;
}

void Response::generateResponse(std::string status)
{
	if (error_page)
		return;
	std::string extension_;
	this->responseStatus = status;
	if (status == "400" || status == "403" || status == "405" ||
		status == "406" || status == "408" || status == "411" ||
		status == "413" || status == "414" || status == "415" ||
		status == "416" || status == "500" || status == "501" ||
		status == "502" || status == "504" || status == "505" ||
		status == "507")
		this->closeClient = true;
	size_t pos = this->Config->ErrorPage.find_last_of(".");
	if (pos != std::string::npos)
		extension_ = this->Config->ErrorPage.substr(pos + 1, this->Config->ErrorPage.length() - pos + 1);
	this->fptr = fopen(this->Config->ErrorPage.c_str(), "rb");
	if (!this->fptr || (extension_ != "html" && extension_ != "txt"))
	{
		stringstream body;
		body << GENERATE_ERROR_HTML(status, Config->status.getStatus(status));
		this->BodyResponse = body.str();
		std::stringstream header_;
		header_ << "HTTP/1.1 " + status + " " + this->Config->status.getStatus(status) + "\r\n";
		header_ << "Content-Type: " + this->Config->mime.getMimeType("html") + "\r\n";
		header_ << "Content-Length: " + intToString(body.str().length()) + "\r\n";
		header_ << "Connection: close\r\n";
		header_ << "\r\n";
		this->HeaderResponse = header_.str();
		this->responseDone = true;
		this->error_page = true;
		return;
	}
	else
	{
		fseek(this->fptr, 0, SEEK_END);
		this->fileSize = ftell(this->fptr);
		fseek(this->fptr, 0, SEEK_SET);
		std::stringstream header_;
		header_ << "HTTP/1.1 " + status + " " + this->Config->status.getStatus(status) + "\r\n";
		header_ << "Content-Type: " + this->Config->mime.getMimeType(extension_) + "\r\n";
		header_ << "Content-Length: " + intToString(this->fileSize) + "\r\n\r\n";
		this->HeaderResponse = header_.str();
		this->BodyResponse.clear();
		this->responseDone = true;
		this->error_page = true;
	}
	return;
}

void Response::handleScriptCGI(Client &client)
{
	if (!CgiRunning && !responseDone)
	{
		CgiRunning = true;
		if (client.request.Method == "POST")
			if ((tempFD = open(client.request.files[0].fileName.c_str(), O_RDONLY)) == -1)
				return (generateResponse("500"));
		if (pipe(pipefd) == -1)
			return (generateResponse("500"));
		pid = fork();
		if (pid == -1)
			return (generateResponse("500"));
		if (pid == 0)
		{
			close(pipefd[0]);
			dup2(pipefd[1], STDOUT_FILENO);
			close(pipefd[1]);
			std::stringstream iss;
			iss << this->Config->CgiTimeout;
			unsigned int alarmValue;
			iss >> alarmValue;
			if (iss.fail())
				return (generateResponse("500"));
			alarm(alarmValue);
			if (tempFD != -1 && client.request.Method == "POST")
				dup2(tempFD, STDIN_FILENO);
			close(tempFD);
			close(2);
			char **envp = new char *[env.size() + 1];
			int i = 0;
			std::multimap<std::string, std::string>::iterator it = env.begin();
			for (; it != env.end(); it++)
			{
				std::string env_entry = it->first + "=" + it->second;
				envp[i] = new char[env_entry.size() + 1];
				std::strcpy(envp[i], env_entry.c_str());
				i++;
			}
			envp[i] = NULL;
			std::cerr << this->route.CgiExec.c_str() << std::endl;
			if (this->route.CgiExec != "default")
			{
				char *const args[] = {(char *)this->route.CgiExec.c_str(), (char *)this->fullpath.c_str(), NULL};
				execve(this->route.CgiExec.c_str(), args, envp);
			}
			else if (this->extension == "py")
			{
				char *const args[] = {(char *)this->Config->pythonCgi.c_str(), (char *)this->fullpath.c_str(), NULL};
				execve(this->Config->pythonCgi.c_str(), args, envp);
			}
			else if (this->extension == "pl")
			{
				char *const args[] = {(char *)this->Config->perlCgi.c_str(), (char *)this->fullpath.c_str(), NULL};
				execve(this->Config->perlCgi.c_str(), args, envp);
			}
			else if (this->extension == "rb")
			{
				char *const args[] = {(char *)this->Config->rubyCgi.c_str(), (char *)this->fullpath.c_str(), NULL};
				execve(this->Config->rubyCgi.c_str(), args, envp);
			}
			else if (this->extension == "php")
			{
				char *const args[] = {(char *)this->Config->phpCgi.c_str(), (char *)this->fullpath.c_str(), NULL};
				execve(this->Config->phpCgi.c_str(), args, envp);
			}
			for (int i = 0; envp[i] != NULL; i++)
				delete[] envp[i];
			delete[] envp;
			exit(EXIT_FAILURE);
		}
		close(pipefd[1]);
		if (fcntl(pipefd[0], F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1)
			return (generateResponse("500"));
		char tempFile[] = "/tmp/.cgi_body_XXXXXXXXXXX";
		FDCGIBody = mkstemp(tempFile);
		if (FDCGIBody)
			tempFileName = tempFile;
	}
	char buffer[4096];
	ssize_t bytesRead;
	resCgi.clear();
	while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0)
		resCgi.append(buffer, bytesRead);
	if (resCgi.length() > 0)
	{
		ssize_t sizew = write(FDCGIBody, resCgi.c_str(), resCgi.length());
		if (sizew == -1)
			return (generateResponse("500"));
	}
	if (bytesRead == 0)
	{
		resCgi.clear();
		if (!this->headerCgiReady)
		{
			std::ifstream infile(tempFileName);
			std::stringstream responseStream, bodyStream;
			responseStream << infile.rdbuf();
			std::string resHeaders__, resBody__, line;
			while (std::getline(responseStream, line))
			{
				if (line.empty() || line == "\r")
					break;
				resHeaders__ += line + "\n";
			}
			MAPhederscgi = parseResponseHeader(resHeaders__);
			if (this->responseDone)
			{
				close(pipefd[0]);
				close(FDCGIBody);
				if (tempFD != -1 && client.request.Method == "POST")
					close(tempFD), unlink(client.request.files[0].fileName.c_str());
				return;
			}
			std::streampos Pos = responseStream.tellg();
			infile.seekg(Pos, std::ios::beg);
			bodyStream << infile.rdbuf();
			close(FDCGIBody);
			unlink(tempFileName.c_str());
			FDCGIBody = -1;
			char tempFile[] = "/tmp/.cgi_body_XXXXXXXXXXX";
			FDCGIBody = mkstemp(tempFile);
			if (FDCGIBody == -1)
				return (generateResponse("500"));
			tempFileName = tempFile;
			ssize_t sizeW = write(FDCGIBody, bodyStream.str().c_str(), bodyStream.str().length());
			if (sizeW == -1)
				return (generateResponse("500"));
			this->headerCgiReady = true;
		}
		else
		{
			int status;
			pid_t result = waitpid(pid, &status, WNOHANG);
			if (result == -1)
				return (generateResponse("500"));
			if (result != 0)
			{
				this->responseDone = true;
				this->CgiRunning = false;
				close(FDCGIBody);
				close(pipefd[0]);
				if (tempFD != -1 && client.request.Method == "POST")
					close(tempFD), unlink(client.request.files[0].fileName.c_str());
				if (WIFSIGNALED(status) && WTERMSIG(status) == SIGALRM)
					return (generateResponse("504"));
				std::multimap<std::string, std::string>::iterator it = MAPhederscgi.find("Status");
				std::multimap<std::string, std::string>::iterator end = MAPhederscgi.end();
				std::string Rstatus;
				if (it != end)
				{
					std::vector<std::string> vec = splitString(it->second, " ");
					if (vec.size() > 0)
						Rstatus = vec[0];
				}
				if (WIFEXITED(status) && (WEXITSTATUS(status) == 0 || !Rstatus.empty()))
				{
					this->fptr = fopen(this->tempFileName.c_str(), "rb");
					if (!this->fptr)
						return (generateResponse("500"));
					fseek(this->fptr, 0, SEEK_END);
					this->fileSize = ftell(this->fptr);
					fseek(this->fptr, 0, SEEK_SET);
					std::string resHeaders__;
					resHeaders__ = generateResponseHeaderCGI(MAPhederscgi, this->fileSize, Rstatus);
					this->HeaderResponse = resHeaders__;
					this->responseDone = true;
					return;
				}
				else
					return (generateResponse("502"));
			}
		}
	}
}

std::multimap<std::string, std::string> Response::parseResponseHeader(std::string header)
{
	std::multimap<std::string, std::string> headers;
	std::istringstream stream(header);
	std::string line;
	while (std::getline(stream, line))
	{
		if (line.empty() || line == "\r")
			break;
		size_t pos = line.find(':');
		if ((pos == std::string::npos) ? (generateResponse("502"), true) : false)
			break;
		std::string key = line.substr(0, pos);
		std::string value = line.substr(pos + 1, line.length());
		headers.insert(std::make_pair(strToLower(key), trim(value, " \t\r\n")));
	}
	return headers;
}

std::string Response::generateResponseHeaderCGI(std::multimap<std::string, std::string> &headers, size_t body_lenght, std::string Rstatus)
{
	std::stringstream header_;
	if (Rstatus.empty())
	{
		this->responseStatus = "200";
		header_ << "HTTP/1.1 " << 200 << " " << this->Config->status.getStatus("200") << "\r\n";
	}
	else if (!this->Config->status.CheckStatus(Rstatus))
		return (generateResponse("502"), this->HeaderResponse);
	else
	{
		this->responseStatus = Rstatus;
		header_ << "HTTP/1.1 " << Rstatus << " " << this->Config->status.getStatus(Rstatus) << "\r\n";
	}
	for (std::multimap<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
		header_ << it->first + ": " + it->second + "\r\n";
	if (headers.find("content-length") == headers.end())
		header_ << "Content-Length: " << body_lenght << "\r\n";
	header_ << "\r\n";
	return header_.str();
}

void Response::mergeHeadersValuesCGI(Client &client)
{
	if (this->headerMerged)
		return;
	this->headerMerged = true;
	std::multimap<std::string, std::string> newHeaders;
	std::multimap<std::string, std::string>::iterator it = client.request.mapHeaders.begin();
	std::multimap<std::string, std::string>::iterator end = client.request.mapHeaders.end();
	for (; it != end; it++)
	{
		const std::string &key = it->first;
		std::multimap<std::string, std::string>::iterator mergedIt;
		mergedIt = newHeaders.find(key);
		if (mergedIt == newHeaders.end())
			newHeaders.insert(std::make_pair(key, it->second));
		else
			mergedIt->second += ", " + it->second;
	}
	client.request.mapHeaders = newHeaders;
}

void Response::generateEnvCGI(Client &client)
{
	if (this->envCgiReady)
		return;
	this->envCgiReady = true;
	std::multimap<std::string, std::string> env_;
	std::string SERVER_SOFTWARE = "webserv";
	std::string SERVER_NAME = this->Config->ServerNames;
	std::string GATEWAY_INTERFACE = "CGI/1.1";
	std::string SERVER_PROTOCOL = "HTTP/1.1";
	std::string SERVER_PORT = this->Config->Port;
	std::string REQUEST_METHOD = client.request.Method;
	std::string PATH_INFO = this->path_info;
	std::string PATH_TRANSLATED = this->path_translated;
	std::string SCRIPT_NAME = this->fullpath;
	std::string SCRIPT_FILENAME = this->fullpath;
	std::string QUERY_STRING = this->query;
	std::string REMOTE_ADDR = client.clientIP;
	std::string CONTENT_TYPE;
	if (client.request.mapHeaders.find("content-type") != client.request.mapHeaders.end())
		CONTENT_TYPE = client.request.mapHeaders.find("content-type")->second;
	std::string CONTENT_LENGTH;
	if (client.request.mapHeaders.find("content-length") != client.request.mapHeaders.end())
		CONTENT_LENGTH = client.request.mapHeaders.find("content-length")->second;
	std::string REDIRECT_STATUS = "CGI";
	env_.insert(std::make_pair("SERVER_SOFTWARE", SERVER_SOFTWARE));
	env_.insert(std::make_pair("SERVER_NAME", SERVER_NAME));
	env_.insert(std::make_pair("GATEWAY_INTERFACE", GATEWAY_INTERFACE));
	env_.insert(std::make_pair("SERVER_PROTOCOL", SERVER_PROTOCOL));
	env_.insert(std::make_pair("SERVER_PORT", SERVER_PORT));
	env_.insert(std::make_pair("REQUEST_METHOD", REQUEST_METHOD));
	env_.insert(std::make_pair("PATH_INFO", PATH_INFO));
	env_.insert(std::make_pair("PATH_TRANSLATED", PATH_TRANSLATED));
	env_.insert(std::make_pair("SCRIPT_NAME", SCRIPT_NAME));
	env_.insert(std::make_pair("SCRIPT_FILENAME", SCRIPT_FILENAME));
	env_.insert(std::make_pair("QUERY_STRING", QUERY_STRING));
	env_.insert(std::make_pair("CONTENT_TYPE", CONTENT_TYPE));
	env_.insert(std::make_pair("CONTENT_LENGTH", CONTENT_LENGTH));
	env_.insert(std::make_pair("REMOTE_ADDR", REMOTE_ADDR));
	env_.insert(std::make_pair("REDIRECT_STATUS", REDIRECT_STATUS));
	std::multimap<std::string, std::string>::iterator it;
	std::multimap<std::string, std::string>::iterator end;
	it = client.request.mapHeaders.begin();
	end = client.request.mapHeaders.end();
	std::string keyEnv, valueEnv;
	for (; it != end; it++)
	{
		if (it->first == "content-type" || it->first == "content-length")
			continue;
		std::string keyEnv = "HTTP_" + it->first;
		std::string::iterator ch = keyEnv.begin();
		for (; ch != keyEnv.end(); ++ch)
			*ch = std::toupper(*ch);
		for (std::string::iterator ch = keyEnv.begin(); ch != keyEnv.end(); ch++)
			*ch = (*ch == '-') ? '_' : *ch;
		std::string valueEnv = it->second;
		env_.insert(std::pair<std::string, std::string>(keyEnv, valueEnv));
	}
	this->env = env_;
	return;
}

Response::Response()
{
	// CGI
	CgiRunning = false;
	headerCgiReady = false;
	envCgiReady = false;
	headerMerged = false;
	tempFD = -1;
	FDCGIBody = -1;
	resCgi.clear();
	tempFileName.clear();
	MAPhederscgi.clear();
	// RESPONSE
	isCgi = false;
	isBodySent = false;
	closeClient = false;
	isHeaderSent = false;
	responseDone = false;
	responseSent = false;
	method_allowd = false;
	head_method = false;
	error_page = false;
	match = 0;
	offset = 0;
	fileSize = 0;
	responseStatus.clear();
	responseString.clear();
	HeaderResponse.clear();
	BodyResponse.clear();
	query.clear();
	path_info.clear();
	path_translated.clear();
	fullpath.clear();
	extension.clear();
	entryPath.clear();
	root.clear();
	env.clear();
	fptr = NULL;
	Config = NULL;
}

void Response::clear()
{
	// CGI
	CgiRunning = false;
	headerCgiReady = false;
	headerMerged = false;
	envCgiReady = false;
	tempFD = -1;
	FDCGIBody = -1;
	resCgi.clear();
	if (!tempFileName.empty())
		unlink(tempFileName.c_str());
	tempFileName.clear();
	MAPhederscgi.clear();
	// RESPONSE
	isCgi = false;
	isBodySent = false;
	closeClient = false;
	isHeaderSent = false;
	responseDone = false;
	responseSent = false;
	method_allowd = false;
	head_method = false;
	error_page = false;
	match = 0;
	offset = 0;
	fileSize = 0;
	responseStatus.clear();
	responseString.clear();
	HeaderResponse.clear();
	BodyResponse.clear();
	query.clear();
	path_info.clear();
	path_translated.clear();
	fullpath.clear();
	extension.clear();
	entryPath.clear();
	root.clear();
	env.clear();
	if (fptr)
		fclose(fptr);
	fptr = NULL;
	Config = NULL;
}

Response::~Response() {}
