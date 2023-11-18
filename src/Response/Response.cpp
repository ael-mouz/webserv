#include "../../include/Response/Response.hpp"
#include "../../include/Config/ServerConfig.hpp"
#include "../../include/Server/Client.hpp"

Response::Response()
{
	// CGI
	CgiRunning = false;
	headerCgiReady = false;
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

Response::~Response() {}

void Response::clear()
{
	// CGI
	CgiRunning = false;
	headerCgiReady = false;
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

void Response::handleRange(stringstream &header, const std::string &range)
{
	size_t equalsPos = range.find('=');
	size_t hyphenPos = range.find('-');
	if (equalsPos == std::string::npos || hyphenPos == std::string::npos)
	{
		generateResponse("416");
		this->responseDone = true;
		return;
	}
	this->offset = 0;
	std::string startByteStr = range.substr(equalsPos + 1, hyphenPos - equalsPos - 1);
	std::istringstream(startByteStr) >> this->offset;
	if (this->offset >= this->fileSize)
	{
		generateResponse("416");
		this->responseDone = true;
		return;
	}
	this->responseStatus = "206";
	header << "HTTP/1.1 206 " << this->Config->status.getStatus("206") << "\r\n";
	header << "Accept-Ranges: bytes\r\n";
	header << "Content-Type: " << this->Config->mime.getMimeType(this->extension) << "\r\n";
	header << "Content-Range: bytes " << this->offset << "-" << (this->fileSize - 1) << "/" << this->fileSize << "\r\n";
	fseek(this->fptr, this->offset, SEEK_SET);
	this->fileSize -= this->offset;
}

void Response::handleNormalFiles(Client &client)
{
	if (this->responseDone)
		return;
	// std::cout << "▻NormalFiles◅ ----------------------------------------------------" << std::endl;
	this->fptr = fopen(this->fullpath.c_str(), "rb");
	if (!this->fptr)
	{
		std::cout << "here" << std::endl;
		generateResponse("500");
		this->responseDone = true;
		return;
	}
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
		header << "HTTP/1.1 200 OK\r\n";
		header << "Accept-Ranges: bytes\r\n";
		header << "Content-Type: " << this->Config->mime.getMimeType(this->extension) << "\r\n";
	}
	header << "Content-Length: " << fileSize << "\r\n";
	header << "\r\n";
	this->HeaderResponse = header.str();
	this->responseDone = true;
}

void Response::sendResponse(Client &client)
{
	responseString.clear();
	if (responseDone && !HeaderResponse.empty() && !isBodySent)
	{
		if (!isHeaderSent)
		{
			responseString = HeaderResponse;
			isHeaderSent = true;
			std::string status;
			std::string StringStatus;
			if (responseStatus >= "400")
			{
				status = "[" FG_RED + responseStatus + RESET_ALL + "]";
				StringStatus = "[" FG_RED + this->Config->status.getStatus(responseStatus) + RESET_ALL + "]";
			}
			else
			{
				status = "[" FG_GREEN + responseStatus + RESET_ALL + "]";
				StringStatus = "[" FG_GREEN + this->Config->status.getStatus(responseStatus) + RESET_ALL + "]";
			}
			std::string URI = "[" FG_YELLOW + this->fullpath + RESET_ALL + "]";
			logMessage(SINFO, client.clientHost, client.socketClient, " URI : " + URI);
			logMessage(SRES, client.clientHost, client.socketClient, status + " " + StringStatus + " Response sent to " + client.clientIP);
			// std::cout << convertText(HeaderResponse) << std::endl;
			if(client.request.Method == "HEAD")
				head_method = true;
		}
		if (!BodyResponse.empty() && !head_method)
		{
			responseString += BodyResponse;
			responseSent = isBodySent = true;
		}
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
			if (send(client.socketClient, responseString.c_str(), responseString.length(), 0) < 0)
				closeClient = responseSent = true;
			// std::stringstream ss;
			// time_t currentTime = time(NULL);
			// ss << "file" << currentTime;
			// std::ofstream file(ss.str().c_str());
			// file << "--------------------------------------------------" << std::endl;
			// file << convertText(responseString.c_str()) << std::endl;
			// file << "--------------------------------------------------" << std::endl;
			// file.close();
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
	// if (this->route.Accepted_Methods == "on")
	// {
	// 	if (client.request.Method == this->route.Accepted_Methods_ ||
	// 		client.request.Method == this->route.Accepted_Methods__ ||
	// 		client.request.Method == this->route.Accepted_Methods___)
	// 		method_allowd = true;
	// }
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
		headers << "HTTP/1.1 204 " + this->Config->status.getStatus("204") + "\r\n";
		headers << "Content-Type: " << this->Config->mime.getMimeType("html") << "\r\n";
		headers << "Content-Length: " << 0 << "\r\n";
		headers << "\r\n";
		this->HeaderResponse = headers.str();
		this->responseDone = true;
		return;
	}
	if (client.request.ReqstDone == 201)
	{
		std::stringstream headers;
		this->responseStatus = "201";
		std::stringstream body_;
		body_ << "<!DOCTYPE html>";
		body_ << "<html lang=\"en\">";
		body_ << "<head>";
		body_ << "<title>File Upload Status</title>";
		body_ << "<style>";
		body_ << "body {";
		body_ << "font-family: Arial, sans-serif;";
		body_ << "margin: 0;";
		body_ << "padding: 0;";
		body_ << "background-image: url(\"/images/site42-bg.png\");";
		body_ << "background-size: cover;";
		body_ << "background-position: center;";
		body_ << "}";
		body_ << ".upload-status {";
		body_ << "max-width: 600px;";
		body_ << "margin: 20px auto;";
		body_ << "padding: 20px;";
		body_ << "background-color: #fff;";
		body_ << "box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);";
		body_ << "text-align: center;";
		body_ << "}";
		body_ << "ul {";
		body_ << "list-style-type: none;";
		body_ << "padding: 0;";
		body_ << "}";
		body_ << "li {";
		body_ << "margin-bottom: 8px;";
		body_ << "}";
		body_ << "</style>";
		body_ << "</head>";
		body_ << "<body>";
		body_ << "<section class=\"upload-status\">";
		body_ << "<h2>Status: Created (201)</h2>";
		body_ << "<p>The following files were created:</p>";
		body_ << "<ul>";
		vector<File>::iterator it = client.request.files.begin();
		vector<File>::iterator end = client.request.files.end();
		for (; it != end; it++)
			body_ << "<li>" + it->fileName + "</li>\n ";
		body_ << "</ul>";
		body_ << "</section>";
		body_ << "</body>";
		body_ << "</html>";
		headers << "HTTP/1.1 201 " + this->Config->status.getStatus("201") + "\r\n";
		headers << "Content-Type: " << this->Config->mime.getMimeType("html") << "\r\n";
		headers << "Content-Length: " << body_.str().length() << "\r\n";
		headers << "\r\n";
		this->HeaderResponse = headers.str();
		this->BodyResponse = body_.str();
		this->responseDone = true;
		return;
	}
	else if (client.request.ReqstDone != 200)
	{
		generateResponse(intToString(client.request.ReqstDone));
		this->responseDone = true;
		return;
	}
}

void Response::CGI(Client &client)
{
	mergeHeadersValuesCGI(client);
	generateEnvCGI(client);
	handleScriptCGI(client);
}

void Response::getConfig(Client &client)
{
	// std::cout << "▻Get config◅ -----------------------------------------------------" << std::endl;
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
	// std::cout << "▻Regenerate extension◅ -------------------------------------------" << std::endl;
	size_t pos5 = this->fullpath.find_last_of(".");
	if (pos5 != std::string::npos)
		this->extension = this->fullpath.substr(pos5 + 1, this->fullpath.length() - pos5 + 1);
	if (this->extension == "pl" || this->extension == "py" ||
		this->extension == "php" || this->extension == "rb")
		this->isCgi = true;
}

void Response::parseUri(std::string uri) // TODO:: uri empty ?/?
{
	// std::cout << "▻Parse uri◅ ------------------------------------------------------" << std::endl;
	size_t posquery = uri.find("?");
	if (posquery != std::string::npos)
	{
		this->query = uri.substr(posquery + 1, uri.length() - posquery);
		uri = uri.substr(0, posquery);
	}
	std::vector<std::string> parts = splitString(uri, "/");
	std::vector<std::string> resolvedPath;
	for (std::vector<std::string>::const_iterator it = parts.begin(); it != parts.end(); it++)
		if (*it != ".." && *it != "." && !it->empty())
			resolvedPath.push_back(*it);
		else if (!resolvedPath.empty() && *it != "." && !it->empty())
			resolvedPath.pop_back();
	std::string path;
	for (std::vector<std::string>::const_iterator it2 = resolvedPath.begin(); it2 != resolvedPath.end(); it2++)
		path += '/' + *it2;
	if(uri[uri.length() - 1] == '/')
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
		{
			this->path_info = uri.substr(pos1, uri.length() - pos1);
			this->fullpath = uri.substr(0, pos1);
		}
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
	// std::cout << "▻Get Route◅ -----------------------------------------------------" << std::endl;
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
			{
				this->route = this->Config->getRoute(dir);
				match = 0;
					// std::cout << "Default match     : " << this->route.RoutePath << std::endl;
			}
			else
			{
				match = 1;
				// std::cout << "Directory match   : " << this->route.RoutePath << std::endl;
			}
		}
		else
		{
			match = 2;
			// std::cout << "Extention match   : " << this->route.RoutePath << std::endl;
		}
	}
	else
	{
		match = 3;
		// std::cout << "Full match        : " << this->route.RoutePath << std::endl;
	}
}
void Response::genrateRederiction(Client &client)
{
	if (this->responseDone)
		return;
	// std::cout << "▻Generate redirection◅ -----------------------------------------------------" << std::endl;
	if (this->route.Redirection == "on")
	{
		std::stringstream Headers__;
		Headers__ << "HTTP/1.1 " + this->route.RedirectionStatus + " " + this->Config->status.getStatus(this->route.RedirectionStatus) + " \r\n";
		Headers__ << "Location: " + this->route.RedirectionURL + "\r\n";
		Headers__ << "Content-Length: 0\r\n";
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
        (void)client;
		std::stringstream Headers__;
		this->responseStatus = "302";
		Headers__ << "HTTP/1.1 302 " << this->Config->status.getStatus("302") << "\r\n";
		Headers__ << "Location: http://" + this->Config->Host + ":" + this->Config->Port + this->entryPath + "/\r\n";
		Headers__ << "Content-Length: 0\r\n";
		Headers__ << "\r\n";
		this->HeaderResponse = Headers__.str();
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
		{
			this->responseDone = true;
			return;
		}
		if (this->route.Autoindex == "on")
		{
			generateAutoIndex();
			this->responseDone = true;
			return;
		}
		if (this->route.Index == "default")
			this->fullpath += "/index.html";
		else
			this->fullpath = this->fullpath + "/" + this->route.Index;
		dirr = isDirectory(this->fullpath.c_str());
		if (dirr == -1 || dirr == 1)
		{
			generateResponse("404");
			this->responseDone = true;
			return;
		}
	}
	else if (dirr == -1)
	{
		generateResponse("404");
		this->responseDone = true;
		return;
	}
}

void Response::getFULLpath()
{
	if (this->responseDone)
		return;
	// std::cout << "▻Get full path◅ --------------------------------------------------" << std::endl; // TODO:: CHECK nginx
	if (route.Root != "default" && route.RoutePath != "default" && this->match == 1)
		this->entryPath = this->fullpath.substr(route.RoutePath.size()),
		this->fullpath = this->route.Root + this->fullpath.substr(route.RoutePath.size()),
		this->path_translated = this->route.Root + this->path_info,
        this->root = this->route.Root;
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
}

void Response::handleScriptCGI(Client &client)
{
	// std::cout << "▻Run Cgi◅ --------------------------------------------------------" << std::endl;
	if (client.request.Method == "DELETE")
	{
		generateResponse("501");
		return;
	}
	if (!CgiRunning && !responseDone)
	{
		CgiRunning = true;
		if (client.request.Method == "POST")
		{
			// std::cout << "FILE : " << client.request.files[0].fileName << std::endl;
			tempFD = open(client.request.files[0].fileName.c_str(), O_RDONLY);
			if (tempFD == -1)
			{
				generateResponse("500");
				responseDone = true;
				return;
			}
		}
		if (pipe(pipefd) == -1)
		{
			generateResponse("500");
			responseDone = true;
			return;
		}
		pid = fork();
		// pid_t pid = fork();
		if (pid == -1)
		{
			generateResponse("500");
			responseDone = true;
			return;
		}
		if (pid == 0)
		{
			close(pipefd[0]);
			dup2(pipefd[1], STDOUT_FILENO);
			close(pipefd[1]);
			alarm(50);
			if (tempFD != -1 && client.request.Method == "POST")
				dup2(tempFD, STDIN_FILENO);
			close(tempFD);
			char **envp = new char *[env.size() + 1];
			int i = 0;
			for (std::multimap<std::string, std::string>::iterator it = env.begin(); it != env.end(); it++)
			{
				std::string env_entry = it->first + "=" + it->second;
				envp[i] = new char[env_entry.size() + 1];
				std::strcpy(envp[i], env_entry.c_str());
				i++;
			}
			envp[i] = NULL;
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
				execve(this->Config->phpCgi.c_str(), args, envp); // must change
			}
			// perror("execve failed");
			for (int i = 0; envp[i] != NULL; i++)
				delete[] envp[i];
			delete[] envp;
			exit(EXIT_FAILURE);
		}
		close(pipefd[1]);
		if (fcntl(pipefd[0], F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1)
		{
			perror("fcntl");
		}
		char tempFile[] = "/tmp/.cgi_body_XXXXXXXXXXX";
		FDCGIBody = mkstemp(tempFile);
		tempFileName = tempFile;
	}
	char buffer[4096];
	ssize_t bytesRead;
	while ((bytesRead = read(pipefd[0], buffer, 4096)) > 0)
		resCgi.append(buffer, bytesRead);
	if (!this->headerCgiReady)
	{
		size_t pos = resCgi.find("\r\n\r\n");
		if (pos != std::string::npos)
		{
			std::istringstream responseStream(resCgi);
			std::string resHeaders__, resBody__;
			std::string line;
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
				return;
			}
			resBody__ = resCgi.substr(pos + 4, resCgi.length() - pos + 4);
			write(FDCGIBody, resBody__.c_str(), resBody__.length());
			this->headerCgiReady = true;
			resCgi.clear();
		}
	}
	else
	{
		write(FDCGIBody, resCgi.c_str(), resCgi.length());
	}
	if (bytesRead <= 0)
	{
		int status;
		pid_t result = waitpid(pid, &status, WNOHANG);
		if (result != 0 && result != -1)
		{
			this->responseDone = true;
			CgiRunning = false;
			close(FDCGIBody);
			close(pipefd[0]);
			if (tempFD != -1 && client.request.Method == "POST")
			{
				close(tempFD);
				unlink(client.request.files[0].fileName.c_str());
			}
			if (WIFSIGNALED(status) && WTERMSIG(status) == SIGALRM)
			{
				generateResponse("504");
				this->responseDone = true;
				return;
			}
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
				{
					generateResponse("500");
					this->responseDone = true;
					return;
				}
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
			{
				generateResponse("502");
				this->responseDone = true;
				return;
			}
		}
	}
}

void Response::generateResponse(std::string status)
{
	if (error_page) // check this again
		return;
	std::string extension_;
	this->responseStatus = status;
	// 400 403 406 407 411 413 416 500 502 504 505;
	if (status == "400" || status == "403" || status == "406" || status == "405" ||
         status == "407" || status == "408" ||
		status == "411" || status == "413" || status == "416" || status == "500" ||
		status == "502" || status == "504" || status == "505" || status == "507")
		this->closeClient = true;
	size_t pos = this->Config->ErrorPage.find_last_of(".");
	if (pos != std::string::npos)
		extension_ = this->Config->ErrorPage.substr(pos + 1, this->Config->ErrorPage.length() - pos + 1);
	// std::ifstream infile(this->Config->ErrorPage.c_str());
	this->fptr = fopen(this->Config->ErrorPage.c_str(), "rb");
	if (!this->fptr || (extension_ != "html" && extension_ != "txt"))
	{
		std::stringstream body;
		body << "<!DOCTYPE html>\n";
		body << "<html lang=\"en\">\n";
		body << "<head>\n";
		body << "<meta charset=\"UTF-8\" />\n";
		body << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" />\n";
		body << "<title>" + status + " Error - " + this->Config->status.getStatus(status) + "</title>\n";
		body << "<style>\n";
		body << "body {\n";
		body << "margin: 0;\n";
		body << "padding: 0;\n";
		body << "font-family: Arial, sans-serif;\n";
		body << "background-image: url('/images/site42-bg.png');\n";
		body << "background-size: cover;\n";
		body << "background-position: center;\n";
		body << "height: 100vh;\n";
		body << "display: flex;\n";
		body << "flex-direction: column;\n";
		body << "justify-content: center;\n";
		body << "align-items: center;\n";
		body << "color: #000000;\n";
		body << "}\n";
		body << "h1 {\n";
		body << "font-size: 4rem;\n";
		body << "margin-bottom: 20px;\n";
		body << "}\n";
		body << "p {\n";
		body << "font-size: 1.5rem;\n";
		body << "text-align: center;\n";
		body << "}\n";
		body << "</style>\n";
		body << "</head>\n";
		body << "<body>\n";
		body << "<h1>Error " + status + "</h1>\n";
		body << "<p>" + this->Config->status.getStatus(status) + "</p>\n";
		body << "</body>\n";
		body << "</html>";
		this->BodyResponse = body.str();
		std::stringstream header_;
		header_ << "HTTP/1.1 " + status + " " + this->Config->status.getStatus(status) + "\r\n";
		header_ << "Content-Type: " + this->Config->mime.getMimeType("html") + "\r\n";
		header_ << "Content-Length: " + intToString(body.str().length()) + "\r\n\r\n";
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
		this->responseDone = true;
		this->error_page = true;
	}
	return;
}

void Response::generateAutoIndex(void)
{
	DIR *dir;
	struct dirent *entry;
	dir = opendir(this->fullpath.c_str());
	if (!dir)
	{
		generateResponse("500");
		this->responseDone = true;
		return;
	}
	std::stringstream autoIndex;
	autoIndex << "<!DOCTYPE html>\n";
	autoIndex << "<html>\n";
	autoIndex << "<head>\n";
	autoIndex << "<meta charset=\"UTF-8\">\n";
	autoIndex << "<title>Index of " << this->fullpath << "</title>\n";
	autoIndex << "<style>\n";
	autoIndex << "body { font-family: Arial, sans-serif;background-image: url(\"/images/site42-bg.gif\");background-size: cover;background-position: center; }\n";
	autoIndex << "table { width: 100%; border-collapse: collapse; }\n";
	autoIndex << "th, td { padding: 10px; text-align: left; border-bottom: 1px solid black;}\n";
	autoIndex << ".icon { width: 20px; height: 20px; margin-right: 10px; }\n";
	autoIndex << ".Directory { color: #0070c0; background:#f2f2f2; }\n";
	autoIndex << ".File { color: #000; }\n";
	autoIndex << ".col { text-align: center; }\n";
	autoIndex << "a {text-decoration : none;}\n ";
	autoIndex << "</style>\n";
	autoIndex << "</head>\n";
	autoIndex << "<body>\n";
	autoIndex << "<table>\n";
	autoIndex << "<tr>\n";
	autoIndex << "<th>Item</th>\n";
	autoIndex << "<th class='col'>Type</th>\n";
	autoIndex << "<th class='col'>Size</th>\n";
	autoIndex << "<th class='col'>Permissions</th>\n";
	autoIndex << "<th class='col'>Last access time</th>\n";
	autoIndex << "<th class='col'>Last modification time</th>\n";
	autoIndex << "<th class='col'>Last status change time</th>\n";
	autoIndex << "</tr>\n";
	while ((entry = readdir(dir)) != NULL)
	{
		if (entry->d_type == DT_DIR && std::strcmp(entry->d_name, ".") == 0)
			continue;
		std::string fileType = (entry->d_type == DT_DIR) ? "Directory" : "File";
		std::string icon = (entry->d_type == DT_DIR) ? "/images/folder.svg" : "/images/file.png";
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
		autoIndex << "<tr>\n";
		autoIndex << "<td class='" << fileType << "'>\n";
		autoIndex << "<img src='" << icon << "' class='icon'><a href='" << link << "' class='" << fileType << "'>" << entry->d_name << "</a>\n";
		autoIndex << "</td>\n";
		autoIndex << "<td class='" << fileType << " col'>" << fileType << "</td>\n";
		if (stat(path.c_str(), &fileStat) == 0)
		{
			std::string sizeStr = humanReadableSize(fileStat.st_size);
			autoIndex << "<td class='" << fileType << " col'>" << sizeStr << "</td>\n";
			autoIndex << "<td class='" << fileType << " col'>" << std::oct << (fileStat.st_mode & 0777) << "</td>\n";
			autoIndex << "<td class='" << fileType << " col'>" << std::ctime(&fileStat.st_atime) << "</td>\n";
			autoIndex << "<td class='" << fileType << " col'>" << std::ctime(&fileStat.st_mtime) << "</td>\n";
			autoIndex << "<td class='" << fileType << " col'>" << std::ctime(&fileStat.st_ctime) << "</td>\n";
		}
		autoIndex << "</tr>\n";
	}
	autoIndex << "</table>\n";
	autoIndex << "</body>\n";
	autoIndex << "</html>\n";
	this->BodyResponse = autoIndex.str();
	std::stringstream header_;
	this->responseStatus = "200";
	header_ << "HTTP/1.1 200 OK\r\n";
	header_ << "Content-Type: text/html; charset=UTF-8\r\n";
	header_ << "Content-length: " + intToString(autoIndex.str().length()) + "\r\n\r\n";
	this->HeaderResponse = header_.str();
	this->responseDone = true;
	closedir(dir);
	return;
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
		if (pos == std::string::npos)
		{
			generateResponse("502");
			this->responseDone = true;
			break;
		}
		std::string key = line.substr(0, pos);
		std::string value = line.substr(pos + 2, line.length() - (pos + 2) - 1);
		headers.insert(std::make_pair(key, value));
	}
	// printMap(headers);
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
	{
		generateResponse("502");
		this->responseDone = true;
		return this->HeaderResponse;
	}
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
	// std::cout << "▻merge Headers Values◅ -------------------------------------------" << std::endl;
	std::multimap<std::string, std::string> newHeaders;
	for (std::multimap<std::string, std::string>::iterator it =
			 client.request.mapHeaders.begin();
		 it != client.request.mapHeaders.end(); ++it)
	{
		const std::string &key = it->first;
		std::multimap<std::string, std::string>::iterator mergedIt =
			newHeaders.find(key);
		if (mergedIt == newHeaders.end())
			newHeaders.insert(std::make_pair(key, it->second));
		else
			mergedIt->second += ", " + it->second;
	}
	client.request.mapHeaders = newHeaders;
}

void Response::generateEnvCGI(Client &client)
{
	// std::cout << "▻Generate Cgi env◅ -----------------------------------------------" << std::endl;
	std::multimap<std::string, std::string> env;
	std::string SERVER_SOFTWARE = "webserv";
	std::string SERVER_NAME = this->Config->ServerNames; // TODO:: maybe i should put the server host instead
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
	if (client.request.mapHeaders.find("content-tength") != client.request.mapHeaders.end())
		CONTENT_LENGTH = client.request.mapHeaders.find("content-tength")->second;
	std::string REDIRECT_STATUS = "CGI";
	env.insert(std::make_pair("SERVER_SOFTWARE", SERVER_SOFTWARE));
	env.insert(std::make_pair("SERVER_NAME", SERVER_NAME));
	env.insert(std::make_pair("GATEWAY_INTERFACE", GATEWAY_INTERFACE));
	env.insert(std::make_pair("SERVER_PROTOCOL", SERVER_PROTOCOL));
	env.insert(std::make_pair("SERVER_PORT", SERVER_PORT));
	env.insert(std::make_pair("REQUEST_METHOD", REQUEST_METHOD));
	env.insert(std::make_pair("PATH_INFO", PATH_INFO));
	env.insert(std::make_pair("PATH_TRANSLATED", PATH_TRANSLATED));
	env.insert(std::make_pair("SCRIPT_NAME", SCRIPT_NAME));
	env.insert(std::make_pair("SCRIPT_FILENAME", SCRIPT_FILENAME));
	env.insert(std::make_pair("QUERY_STRING", QUERY_STRING));
	env.insert(std::make_pair("CONTENT_TYPE", CONTENT_TYPE));
	env.insert(std::make_pair("CONTENT_LENGTH", CONTENT_LENGTH));
	env.insert(std::make_pair("REMOTE_ADDR", REMOTE_ADDR));
	env.insert(std::make_pair("REDIRECT_STATUS", REDIRECT_STATUS));
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
		for (std::string::iterator ch = keyEnv.begin(); ch != keyEnv.end(); ++ch)
			*ch = std::toupper(*ch);
		for (std::string::iterator ch = keyEnv.begin(); ch != keyEnv.end(); ++ch)
		{
			if (*ch == '-')
				*ch = '_';
		}
		std::string valueEnv = it->second;
		env.insert(std::pair<std::string, std::string>(keyEnv, valueEnv));
	}
	this->env = env;
}

void Response::ft_printconfig()
{
	if (this->Config)
	{
		std::cout << "╔═══════════════════════════════════════════════════════════════════════════╗" << std::endl;
		std::cout << "║" BOLD " GLOBAL TABLE " RESET_ALL << std::setw(64) << "║" << std::endl;
		std::cout << "╠═════════════════╦═════════════════════════════════════════════════════════╣" << std::endl;
		std::cout << "║ Port            ║" << std::setw(58) << "▻" + this->Config->Port << "◅║" << std::endl;
		std::cout << "║ Host            ║" << std::setw(58) << "▻" + this->Config->Host << "◅║" << std::endl;
		std::cout << "║ ServerNames     ║" << std::setw(58) << "▻" + this->Config->ServerNames << "◅║" << std::endl;
		std::cout << "║ ErrorPage       ║" << std::setw(58) << "▻" + this->Config->ErrorPage << "◅║" << std::endl;
		std::cout << "║ LimitBodySize   ║" << std::setw(58) << "▻" + this->Config->LimitClientBodySize << "◅║" << std::endl;
		std::cout << "║ GlobalRoot      ║" << std::setw(58) << "▻" + this->Config->GlobalRoot << "◅║" << std::endl;
		std::cout << "╚═════════════════╩═════════════════════════════════════════════════════════╝" << std::endl;
	}
}

void Response::ft_printroute()
{
	std::cout << "↦  ╔═══════════════════════════════════════════════════════════════════════════╗" << std::endl;
	std::cout << "↦  ║" FG_BLUE BOLD " ROUTE " << RESET_ALL << std::setw(71) << "║" << std::endl;
	std::cout << "↦  ╠═════════════════╦═════════════════════════════════════════════════════════╣\n";
	std::cout << "↦  ║ RoutePath       ║" << std::setw(58) << "▻" + this->route.RoutePath << "◅║" << std::endl;
	std::cout << "↦  ║ UploadPath      ║" << std::setw(58) << "▻" + this->route.UploadPath << "◅║" << std::endl;
	std::cout << "↦  ║ Redirection     ║" << std::setw(58) << "▻" + this->route.Redirection << "◅║" << std::endl;
	std::cout << "↦  ║ RedirectStatus  ║" << std::setw(58) << "▻" + this->route.RedirectionStatus << "◅║" << std::endl;
	std::cout << "↦  ║ RedirectionURL  ║" << std::setw(58) << "▻" + this->route.RedirectionURL << "◅║" << std::endl;
	std::cout << "↦  ║ Root            ║" << std::setw(58) << "▻" + this->route.Root << "◅║" << std::endl;
	std::cout << "↦  ║ Autoindex       ║" << std::setw(58) << "▻" + this->route.Autoindex << "◅║" << std::endl;
	std::cout << "↦  ║ Index           ║" << std::setw(58) << "▻" + this->route.Index << "◅║" << std::endl;
	std::cout << "↦  ║ Cgi_Exec        ║" << std::setw(58) << "▻" + this->route.CgiExec << "◅║" << std::endl;
	std::cout << "↦  ║ Methods         ║" << std::setw(58) << "▻" + this->route.Accepted_Methods << "◅║" << std::endl;
	std::cout << "↦  ║ Methods1        ║" << std::setw(58) << "▻" + this->route.Accepted_Methods_ << "◅║" << std::endl;
	std::cout << "↦  ║ Methods2        ║" << std::setw(58) << "▻" + this->route.Accepted_Methods__ << "◅║" << std::endl;
	std::cout << "↦  ║ Methods3        ║" << std::setw(58) << "▻" + this->route.Accepted_Methods___ << "◅║" << std::endl;
	std::cout << "↦  ╚═════════════════╩═════════════════════════════════════════════════════════╝" << std::endl;
}
