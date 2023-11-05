#include "../../include/Response/Response.hpp"
#include "../../include/Config/ServerConfig.hpp"
#include "../../include/Server/Client.hpp"

Response::Response()
{
	this->responseDone = false;
	this->isCgi = false;
	this->isBodySent = false;
	this->isHeaderSent = false;
	this->responseSent = false;
	closeClient = false;
	fileSize = 0;
	offset = 0;
	match = 0;
}

Response::~Response() {
	// fclose(fptr); // maybe leak file discriptor
}

void Response::clear()
{
	responseStatus.clear();
	HeaderResponse.clear();
	BodyResponse.clear();
	query.clear();
	path_info.clear();
	path_translated.clear();
	fullpath.clear();
	extension.clear();
	entryPath.clear();
	isCgi = false;
	isBodySent = false;
	closeClient = false;
	isHeaderSent = false;
	responseDone = false;
	responseSent = false;
	fileSize = 0;
	offset = 0;
	match = 0;
	Config = NULL;
	env.clear();
	fclose(fptr);
	fptr = NULL;
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
	header << "HTTP/1.1 206 " << this->Config->status.getStatus("206") << "\r\n";
	header << "Accept-Ranges: bytes\r\n";
	header << "Content-Type: " << this->Config->mime.getMimeType(this->extension) << "\r\n";
	header << "Content-Range: bytes " << this->offset << "-" << (this->fileSize - 1) << "/" << this->fileSize << "\r\n";
	fseek(this->fptr, this->offset, SEEK_SET);
	this->fileSize -= this->offset;
}

void Response::handleNormalFiles(Client &client)
{
	// std::cout << "▻NormalFiles◅ ----------------------------------------------------" << std::endl;
	this->fptr = fopen(this->fullpath.c_str(), "rb");
	if (!this->fptr || this->extension.empty()) /// TODO: maybe this is not corect
	{
		generateResponse("404");
		this->responseDone = true;
		return;
	}
	fseek(this->fptr, 0, SEEK_END);
	this->fileSize = ftell(this->fptr);
	fseek(this->fptr, 0, SEEK_SET);
	std::stringstream header;
	std::multimap<std::string, std::string>::iterator it = client.request.mapHeaders.find("Range");
	if (it != client.request.mapHeaders.end())
		handleRange(header, it->second);
	else
	{
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
	if (!this->isHeaderSent)
	{
		if (!this->HeaderResponse.empty())
		{
			if (send(client.socketClient, client.response.HeaderResponse.c_str(), client.response.HeaderResponse.length(), 0) <= 0)
			{
				this->closeClient = true, this->responseSent = true, this->isBodySent = true;
				return;
			}
			std::cout << "HEADER SENT TO:      " << client.socketClient << " | "
					  << client.response.Config->Host << ":"
					  << client.response.Config->Port << std::endl;
		}
		this->isHeaderSent = true;
	}
	if (!this->isBodySent && this->isHeaderSent)
	{
		if (!this->BodyResponse.empty())
		{
			if (send(client.socketClient, client.response.BodyResponse.c_str(), client.response.BodyResponse.length(), 0) <= 0)
			{
				this->closeClient = true, this->responseSent = true, this->isBodySent = true;
				return;
			}
			this->responseSent = true, this->isBodySent = true;
		}
		else if (this->fptr)
		{
			if (!feof(this->fptr))
			{
				char buffer[1024];
				ssize_t bytesRead = fread(buffer, 1, sizeof(buffer), this->fptr);
				if (bytesRead > 0)
				{
					if (send(client.socketClient, buffer, bytesRead, 0) <= 0)
					{
						this->closeClient = true, this->responseSent = true, this->isBodySent = true;
						return;
					}
				}
			}
			else
			{
				std::cout << "RESPONSE DONE BODY SENT TO:" << client.socketClient << " | "
						  << client.response.Config->Host << ":"
						  << client.response.Config->Port << std::endl;
				this->responseSent = true;
				this->isBodySent = true;
			}
		}
		else
		{
			std::cout << "RESPONSE DONE NO BODY TO: " << client.socketClient << " | "
					  << client.response.Config->Host << ":"
					  << client.response.Config->Port << std::endl;
			this->responseSent = true;
			this->isBodySent = true;
		}
	}
}

void Response::response(Client &client)
{
	if (responseDone)
		return;
	// std::cout << "▻Start Response◅ -------------------------------------------------" << std::endl;

	mergeHeadersValues(client);
	getConfig(client);
	parseUri(client.request.URI);
	getRoute();
	genrateRederiction();
	if (responseDone)
		return;
	getFULLpath();
	if (responseDone)
		return;
	regenerateExtonsion();
	if (responseDone)
		return;
	if (this->isCgi)
	{
		generateCGIEnv(client);
		handleCGIScript(client);
		if (responseDone)
			return;
	}
	else
	{
		handleNormalFiles(client);
	}
	// std::cout << "▻End Response◅ ---------------------------------------------------" << std::endl;
}

void Response::mergeHeadersValues(Client &client)
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
	// printMap(newHeaders);
}

void Response::getConfig(Client &client)
{
	// std::cout << "▻Get config◅ -----------------------------------------------------" << std::endl;
	this->Config = &client.serverConf.DefaultServerConfig;
	std::multimap<std::string, std::string>::iterator it = client.request.mapHeaders.find("Host");
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
	// if (this->Config)
	// {
	// 	std::cout << "╔═══════════════════════════════════════════════════════════════════════════╗" << std::endl;
	// 	std::cout << "║" BOLD " GLOBAL TABLE " RESET_ALL << std::setw(64) << "║" << std::endl;
	// 	std::cout << "╠═════════════════╦═════════════════════════════════════════════════════════╣" << std::endl;
	// 	std::cout << "║ Port            ║" << std::setw(58) << "▻" + this->Config->Port << "◅║" << std::endl;
	// 	std::cout << "║ Host            ║" << std::setw(58) << "▻" + this->Config->Host << "◅║" << std::endl;
	// 	std::cout << "║ ServerNames     ║" << std::setw(58) << "▻" + this->Config->ServerNames << "◅║" << std::endl;
	// 	std::cout << "║ ErrorPage       ║" << std::setw(58) << "▻" + this->Config->ErrorPage << "◅║" << std::endl;
	// 	std::cout << "║ LimitBodySize   ║" << std::setw(58) << "▻" + this->Config->LimitClientBodySize << "◅║" << std::endl;
	// 	std::cout << "║ GlobalRoot      ║" << std::setw(58) << "▻" + this->Config->GlobalRoot << "◅║" << std::endl;
	// 	std::cout << "╚═════════════════╩═════════════════════════════════════════════════════════╝" << std::endl;
	// }
}

void Response::regenerateExtonsion()
{
	// std::cout << "▻Regenerate extension◅ -------------------------------------------" << std::endl;
	size_t pos5 = this->fullpath.find_last_of(".");
	if (pos5 != std::string::npos)
		this->extension =
			this->fullpath.substr(pos5 + 1, this->fullpath.length() - pos5 + 1);
	if (this->extension == "pl" || this->extension == "py" ||
		this->extension == "php" || this->extension == "rb")
		this->isCgi = true;
	// std::cout << "extension  : " << this->extension << std::endl;
	// std::cout << "query      : " << this->query << std::endl;
	// std::cout << "path info  : " << this->path_info << std::endl;
	// std::cout << "full path  : " << this->fullpath << std::endl;
	// std::cout << "cgi        : " << (this->isCgi ? "true" : "false") << std::endl;
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
	pos = this->fullpath.find_last_of(".");
	if (pos != std::string::npos)
		this->extension =
			this->fullpath.substr(pos + 1, this->fullpath.length() - pos + 1);
	// std::cout << "extension  : " << this->extension << std::endl;
	// std::cout << "query      : " << this->query << std::endl;
	// std::cout << "path info  : " << this->path_info << std::endl;
	// std::cout << "script     : " << this->fullpath << std::endl;
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
			while (this->route.RoutePath == "default" && !dir.empty())
			{
				this->route = this->Config->getRoute(dir);
				dir = getParentDirectories(dir);
			}
			// if (dir.empty() && this->route.RoutePath == "default")
			// 	std::cout << "Default match     : " << this->route.RoutePath << std::endl;
			// else
			// 	std::cout << "Directory match   : " << this->route.RoutePath << std::endl, match = 1;
		}
		// else
		// 	std::cout << "Extention match   : " << this->route.RoutePath << std::endl, match = 2;
	}
	// else
	// 	std::cout << "Full match        : " << this->route.RoutePath << std::endl, match = 3;

	// std::cout << "↦  ╔═══════════════════════════════════════════════════════════════════════════╗" << std::endl;
	// std::cout << "↦  ║" FG_BLUE BOLD " ROUTE " << RESET_ALL << std::setw(71) << "║" << std::endl;
	// std::cout << "↦  ╠═════════════════╦═════════════════════════════════════════════════════════╣\n";
	// std::cout << "↦  ║ RoutePath       ║" << std::setw(58) << "▻" + this->route.RoutePath << "◅║" << std::endl;
	// std::cout << "↦  ║ UploadPath      ║" << std::setw(58) << "▻" + this->route.UploadPath << "◅║" << std::endl;
	// std::cout << "↦  ║ Redirection     ║" << std::setw(58) << "▻" + this->route.Redirection << "◅║" << std::endl;
	// std::cout << "↦  ║ RedirectStatus  ║" << std::setw(58) << "▻" + this->route.RedirectionStatus << "◅║" << std::endl;
	// std::cout << "↦  ║ RedirectionURL  ║" << std::setw(58) << "▻" + this->route.RedirectionURL << "◅║" << std::endl;
	// std::cout << "↦  ║ Root            ║" << std::setw(58) << "▻" + this->route.Root << "◅║" << std::endl;
	// std::cout << "↦  ║ Autoindex       ║" << std::setw(58) << "▻" + this->route.Autoindex << "◅║" << std::endl;
	// std::cout << "↦  ║ Index           ║" << std::setw(58) << "▻" + this->route.Index << "◅║" << std::endl;
	// std::cout << "↦  ║ Cgi_Exec        ║" << std::setw(58) << "▻" + this->route.CgiExec << "◅║" << std::endl;
	// std::cout << "↦  ║ Methods         ║" << std::setw(58) << "▻" + this->route.Accepted_Methods << "◅║" << std::endl;
	// std::cout << "↦  ║ Methods1        ║" << std::setw(58) << "▻" + this->route.Accepted_Methods_ << "◅║" << std::endl;
	// std::cout << "↦  ║ Methods2        ║" << std::setw(58) << "▻" + this->route.Accepted_Methods__ << "◅║" << std::endl;
	// std::cout << "↦  ║ Methods3        ║" << std::setw(58) << "▻" + this->route.Accepted_Methods___ << "◅║" << std::endl;
	// std::cout << "↦  ╚═════════════════╩═════════════════════════════════════════════════════════╝" << std::endl;
}

void Response::genrateRederiction()
{
	// std::cout << "▻Generate redirection◅ -----------------------------------------------------" << std::endl;
	if (this->route.Redirection == "on")
	{
		std::stringstream Headers__;
		Headers__ << "HTTP/1.1 " + this->route.RedirectionStatus + " " + this->Config->status.getStatus(this->route.RedirectionStatus) + " \r\n";
		Headers__ << "Location: " + this->route.RedirectionURL + "\r\n";
		Headers__ << "Content-Length: 0\r\n\r\n";
		this->HeaderResponse = Headers__.str();
		this->responseDone = true;
		return;
	}
	if (this->fullpath.back() == '/')
		this->fullpath.pop_back();
	else if (this->extension.empty())
	{
		std::stringstream Headers__;
		Headers__ << "HTTP/1.1 302" << this->Config->status.getStatus("302") << "\r\n";
		Headers__ << "Location: http://" + this->Config->Host + ":" + this->Config->Port + this->fullpath + "/\r\n";
		Headers__ << "Content-Length: 0\r\n\r\n";
		this->HeaderResponse = Headers__.str();
		this->responseDone = true;
		return;
	}
}

void Response::getFULLpath()
{
	// std::cout << "▻Get full path◅ --------------------------------------------------" << std::endl;
	if (route.Root != "default" && route.RoutePath != "default" && match == 1)
		this->entryPath = this->fullpath.substr(route.RoutePath.size()),
		this->fullpath = route.Root + this->fullpath.substr(route.RoutePath.size()),
		this->path_translated = route.Root + this->path_info; // TODO: handel also the extention find
	else if (route.Root != "default" && route.RoutePath != "default")
		this->entryPath = this->fullpath,
		this->fullpath = route.Root + this->fullpath,
		this->path_translated = route.Root + this->path_info;
	else
		this->entryPath = this->fullpath,
		this->fullpath = this->Config->GlobalRoot + this->fullpath,
		this->path_translated = this->Config->GlobalRoot + this->path_info;
	// std::cout << "FULL PATH         : " << this->fullpath << std::endl;
	int dirr = isDirectory(this->fullpath.c_str());
	if (dirr == 1)
	{
		// std::cout << "TYPE              : directory" << std::endl;
		if (route.Autoindex == "on")
		{
			generateAutoIndex();
			responseDone = true;
			return;
		}
		if (route.Index == "default")
			this->fullpath += "/index.html";
		else
			this->fullpath = this->fullpath + "/" + route.Index;
		std::cout << "THE NEW FULL PATH : " << this->fullpath << std::endl;
	}
	// else if (dirr == 2)
	// std::cout << "TYPE              : file" << std::endl;
	// else
	// std::cout << "TYPE              : invalide not found" << std::endl;
}

void Response::handleCGIScript(Client &client)
{
	// std::cout << "▻Run Cgi◅ --------------------------------------------------------" << std::endl
	int tempFD = -1;
	if (client.request.Method == "POST")
	{
		std::cout << "FILE : " << client.request.files[0].fileName << std::endl;
		tempFD = open(client.request.files[0].fileName.c_str(), O_RDONLY);
		if (tempFD == -1)
		{
			generateResponse("500");
			responseDone = true;
			return;
		}
	}
	int pipefd[2];
	if (pipe(pipefd) == -1)
	{
		generateResponse("500");
		responseDone = true;
		return;
	}
	pid_t pid = fork();
	if (pid == -1)
	{
		generateResponse("500");
		responseDone = true;
		return;
	}
	if (pid == 0)
	{
		close(pipefd[0]);
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
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);
		alarm(5);
		if (tempFD != -1 && client.request.Method == "POST")
			dup2(tempFD, STDIN_FILENO), close(tempFD);
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
		perror("execve failed");
		for (int i = 0; envp[i] != nullptr; i++)
			delete[] envp[i];
		delete[] envp;
		exit(EXIT_FAILURE);
	}
	else
	{
		close(pipefd[1]);
		int status;
		waitpid(pid, &status, 0);
		if (WIFSIGNALED(status) && WTERMSIG(status) == SIGALRM)
		{
			generateResponse("504");
			this->responseDone = true;
			return;
		}
		if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
		{
			std::string resCgi;
			char buffer[4096];
			ssize_t bytesRead;
			while ((bytesRead = read(pipefd[0], buffer, 4096)) > 0)
				resCgi.append(buffer, bytesRead);
			// std::cout << convertText(resCgi) << std::endl;
			/*****************************************/
			std::istringstream responseStream(resCgi);
			std::string resHeaders__, resBody__;
			// std::string responseLine,reshttpVersion, resStatus, resStatusString,;
			// std::getline(responseStream, responseLine);
			// std::vector<std::string> responseParts = splitString(responseLine, " ");
			// if (responseParts.size() >= 3)
			// {
			// 	reshttpVersion = responseParts[0];
			// 	resStatus = responseParts[1];
			// 	resStatusString = responseParts[2];
			// }
			// else
			// 	responseStream.seekg(0, std::ios::beg); /// TODO:: maybe i should do this ???
			std::string line;
			while (std::getline(responseStream, line))
			{
				if (line.empty() || line == "\r")
					break;
				resHeaders__ += line + "\n";
			}
			std::multimap<std::string, std::string> MAPheders = parseResponseHeader(resHeaders__);
			if (this->responseDone)
				return;
			size_t pos = resCgi.find("\r\n\r\n");
			if (pos != std::string::npos)
				resBody__ = resCgi.substr(pos + 4, resCgi.length() - pos + 4);
			resHeaders__ = generateResponseHeaderCGI(MAPheders, resBody__);
			// std::cout << "HEDERS: \n"
			// 		  << convertText(resHeaders__) << std::endl;
			// std::cout << "BODY: \n"
			// 		  << convertText(resBody__) << std::endl;
			/*****************************************/
			// this->HeaderResponse = resCgi;//TODO:: ereas it;
			this->HeaderResponse = resHeaders__;
			this->BodyResponse = resBody__;
			this->responseDone = true;
		}
		else
		{
			generateResponse("502");
			this->responseDone = true;
			return;
		}
		close(pipefd[0]);
	}
	close(tempFD);
	// unlink(client.request.files[0].fileName.c_str());
}

void Response::generateCGIEnv(Client &client)
{
	// std::cout << "▻Generate Cgi env◅ -----------------------------------------------" << std::endl;
	std::multimap<std::string, std::string>::iterator it;
	std::multimap<std::string, std::string>::iterator end;
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
	std::string CONTENT_TYPE;
	it = client.request.mapHeaders.find("Content-Type");
	end = client.request.mapHeaders.end();
	if (it != end)
		CONTENT_TYPE = it->second;
	it = client.request.mapHeaders.find("Content-Length");
	end = client.request.mapHeaders.end();
	std::string CONTENT_LENGTH = "0";
	if (it != end)
		CONTENT_LENGTH = it->second;
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
	// env.insert(std::make_pair("REMOTE_ADDR", REMOTE_ADDR)); // TODO:: generate it from socket
	env.insert(std::make_pair("REDIRECT_STATUS", REDIRECT_STATUS)); // NOTE:php only
	it = client.request.mapHeaders.begin();
	std::string keyEnv, valueEnv;
	for (std::map<std::string, std::string>::iterator it =
			 client.request.mapHeaders.begin();
		 it != client.request.mapHeaders.end(); ++it)
	{
		if (it->first == "Content-Type" || it->first == "Content-Length")
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
	// printMap(env);
	this->env = env;
}

void Response::generateResponse(std::string status)
{
	std::string extension_;
	size_t pos = this->Config->ErrorPage.find_last_of(".");
	if (pos != std::string::npos)
		extension_ = this->Config->ErrorPage.substr(pos + 1, this->Config->ErrorPage.length() - pos + 1);
	std::ifstream infile(this->Config->ErrorPage.c_str());
	if (!infile.is_open() || extension_.empty() || extension_ != "html")
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
		return;
	}
	else
	{
		std::stringstream body;
		body << infile.rdbuf();
		this->BodyResponse = body.str();
		std::stringstream header_;
		header_ << "HTTP/1.1 " + status + " " + this->Config->status.getStatus(status) + "\r\n";
		header_ << "Content-Type: " + this->Config->mime.getMimeType("html") + "\r\n";
		header_ << "Content-Length: " + intToString(body.str().length()) + "\r\n\r\n";
		this->HeaderResponse = header_.str();
		this->responseDone = true;
	}
	infile.close();
	return;
}

std::string humanReadableSize(off_t size)
{
	std::ostringstream sizeStr;
	if (size >= (1 << 30))
		sizeStr << (size / (1 << 30)) << " GB";
	else if (size >= (1 << 20))
		sizeStr << (size / (1 << 20)) << " MB";
	else if (size >= (1 << 10))
		sizeStr << (size / (1 << 10)) << " KB";
	else
		sizeStr << size << " B";
	return sizeStr.str();
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
	autoIndex << "body { font-family: Arial, sans-serif; }\n";
	autoIndex << "table { width: 100%; border-collapse: collapse; }\n";
	autoIndex << "th, td { padding: 10px; text-align: left;}\n";
	autoIndex << "th {border-bottom: 1px solid black }\n";
	autoIndex << "tr:hover { background-color: #f5f5f5; }\n";
	autoIndex << ".icon { width: 20px; height: 20px; margin-right: 10px; }\n";
	autoIndex << ".directory { color: #0070c0;background:#f2f2f2; }\n";
	autoIndex << ".file { color: #000; }\n";
	autoIndex << ".col { text-align: center; }\n";
	// autoIndex << ".col1 { text-align: right; }\n";
	autoIndex << "a {text-decoration : none;}\n ";
	autoIndex << "</style>\n";
	autoIndex << "</head>\n";
	autoIndex << "<body>\n";
	// autoIndex << "<h1>Index of " << this->fullpath << "</h1>\n";
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
		std::string icon = (entry->d_type == DT_DIR) ? "/images/folder.svg" : "/images/file.png";
		autoIndex << "<tr>";
		autoIndex << "<td class='" << ((entry->d_type == DT_DIR) ? "directory" : "file") << "'>";
		if (!this->entryPath.empty() && this->entryPath != "/" && this->entryPath.back() != '/')
			autoIndex << "<img src='" << icon << "' class='icon'><a href='" << this->entryPath + "/" + entry->d_name << "' class='" << ((entry->d_type == DT_DIR) ? "directory" : "file") << "'>" << entry->d_name << "</a></td>\n";
		else if (!this->entryPath.empty() && this->entryPath != "/")
			autoIndex << "<img src='" << icon << "' class='icon'><a href='" << this->entryPath + entry->d_name << "' class='" << ((entry->d_type == DT_DIR) ? "directory" : "file") << "'>" << entry->d_name << "</a></td>\n";
		else
			autoIndex << "<img src='" << icon << "' class='icon'><a href='" << entry->d_name << "' class='" << ((entry->d_type == DT_DIR) ? "directory" : "file") << "'>" << entry->d_name << "</a></td>\n";
		autoIndex << "<td class='" << ((entry->d_type == DT_DIR) ? "directory" : "file") << " col'>" << ((entry->d_type == DT_DIR) ? "Directory" : "File") << "</td>\n";
		struct stat fileStat;
		std::string path;
		if (this->fullpath.back() != '/')
			path = this->fullpath + "/" + entry->d_name;
		else
			path = this->fullpath + entry->d_name;
		// std::cout << path << std::endl;
		if (stat(path.c_str(), &fileStat) == 0)
		{
			std::string fileType = (S_ISDIR(fileStat.st_mode)) ? "directory" : "file";
			std::string sizeStr = humanReadableSize(fileStat.st_size);
			autoIndex << "<td class='" << fileType << " col'>" << sizeStr << "</td>\n";
			autoIndex << "<td class='" << fileType << " col'>" << std::oct << (fileStat.st_mode & 0777) << "</td>\n";
			autoIndex << "<td class='" << fileType << " col'>" << std::ctime(&fileStat.st_atime);
			autoIndex << "<td class='" << fileType << " col'>" << std::ctime(&fileStat.st_mtime);
			autoIndex << "<td class='" << fileType << " col'>" << std::ctime(&fileStat.st_ctime);
			// autoIndex << "<td class='" << ((entry->d_type == DT_DIR) ? "directory" : "file") << " col'>" << fileStat.st_size << " bytes</td>\n";
			// autoIndex << "<td class='" << ((entry->d_type == DT_DIR) ? "directory" : "file") << " col'>" << std::oct << (fileStat.st_mode & 0777) << "</td>\n";
			// autoIndex << "<td class='" << ((entry->d_type == DT_DIR) ? "directory" : "file") << " col'>" << std::ctime(&fileStat.st_atime) << "</td>\n";
			// autoIndex << "<td class='" << ((entry->d_type == DT_DIR) ? "directory" : "file") << " col'>" << std::ctime(&fileStat.st_mtime) << "</td>\n";
			// autoIndex << "<td class='" << ((entry->d_type == DT_DIR) ? "directory" : "file") << " col'>" << std::ctime(&fileStat.st_ctime) << "</td>\n";
		}
	}
	autoIndex << "</tr>\n";
	autoIndex << "</table>\n";
	autoIndex << "</body>\n";
	autoIndex << "</html>\n";
	this->BodyResponse = autoIndex.str();
	std::stringstream header_;
	header_ << "HTTP/1.1 200 OK\r\n";
	header_ << "Content-Type: text/html; charset=UTF-8\r\n";
	header_ << "Content-length: " + intToString(autoIndex.str().length()) + "\r\n\r\n";
	this->HeaderResponse = header_.str();
	closedir(dir);
	this->responseDone = true;
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
	printMap(headers);
	return headers;
}

std::string Response::generateResponseHeaderCGI(std::multimap<std::string, std::string> &headers, std::string &body)
{
	std::stringstream header_;
	header_ << "HTTP/1.1 200 OK\r\n";
	for (std::multimap<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
		header_ << it->first + ": " + it->second + "\r\n";
	if (headers.find("Content-length") == headers.end())
		header_ << "Content-length: " << body.length() << "\r\n";
	header_ << "\r\n";
	return header_.str();
}
