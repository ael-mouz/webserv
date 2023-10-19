/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mouz <ael-mouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/18 16:57:43 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/19 21:17:20 by ael-mouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Response.hpp"
#include "../include/server.hpp"
//"400 Bad Request" // [ ] bad request
//"401 Unauthorized" // NOTE: authontication
//"403 Forbidden" // [ ] permission
//"404 Not Found" // [ ] invalid path
//"405 Method Not Allowed" // [ ] GET with body
//"406 Not Acceptable" // [ ]
//"407 Proxy Authentication Required" // [ ]
//"408 Request Timeout" // [ ]
//"409 Conflict" // [ ]
//"410 Gone" // [ ]
//"411 Length Required" // [ ]
//"412 Precondition Failed" // [ ]
//"413 Payload Too Large" // [ ]
//"414 URI Too Long" // [ ]
//"415 Unsupported Media Type" // [ ]
//"416 Range Not Satisfiable" // [ ]
//"417 Expectation Failed" // [ ]
//"418 I'm a teapot" // [ ]
//"421 Misdirected Request" // [ ]
//"422 Unprocessable Entity" // [ ]
//"423 Locked" // [ ]
//"424 Failed Dependency" // [ ]
//"425 Too Early" // [ ]
//"426 Upgrade Required" // [ ]
//"428 Precondition Required" // [ ]
//"429 Too Many Requests" // [ ]
//"431 Request Header Fields Too Large" // [ ]
//"451 Unavailable For Legal Reasons" // [ ]
//"500 Internal Server Error" // NOTE: server error
//"501 Not Implemented" // [x] "501
//"502 Bad Gateway" // TODO: CGI HERE
//"504 Gateway Timeout" // TODO: CGI HERE
//"505 HTTP Version Not Supported" // [x] "505"
//"503 Service Unavailable" // NOTE :update maintennance
//"506 Variant Also Negotiates" // NOTE: config error
//"507 Insufficient Storage" // NOTE: storage error
//"510 Not Extended" // NOTE: no body
//"Network Authentication Required" // NOTE: network error

void printMap(const std::multimap<std::string, std::string> &map)
{
	std::cout << std::setfill('-') << std::setw(145) << "-" << std::endl;
	std::map<std::string, std::string>::const_iterator it1 = map.begin();
	for (; it1 != map.end(); ++it1)
		std::cout << "|Key:" << std::setfill(' ') << std::setw(30) << it1->first << "| Value: " << std::setw(100) << it1->second << "|" << std::endl;
	std::cout << std::setfill('-') << std::setw(145) << "-" << std::endl;
}

void Response::response(int clientSocket, std::string method, std::string uri, std::string httpVersion, std::string Rheaders, std::string body)
{
	parseUri(uri);
	if (method != "GET" && method != "POST" && method != "DELETE")
	{
		this->responseStatus = "HTTP/1.1 501 Not Implemented";
		std::string response = this->responseStatus + "\r\n"
													  "Content-Length: 15\r\n"
													  "Content-Type: text/plain\r\n"
													  "\r\n"
													  "Not Implemented";
		send(clientSocket, response.c_str(), response.length(), 0);
		return;
	}
	else if (httpVersion != "HTTP/1.1\r")
	{
		std::cout << convertText("|" + httpVersion + "|") << std::endl;
		this->responseStatus = "HTTP/1.1 505 HTTP Version Not Supported";
		std::string response = this->responseStatus + "\r\n"
													  "Content-Length: 26\r\n"
													  "Content-Type: text/plain\r\n"
													  "\r\n"
													  "HTTP Version Not Supported";
		send(clientSocket, response.c_str(), response.length(), 0);
		return;
	}
	std::multimap<std::string, std::string> headers = parseHeader(clientSocket, Rheaders); // [x] : parse headers
	headers = mergeHeadersValues(headers);												   // [x] : merge duplicated headers
	std::multimap<std::string, std::string> env = generateCGIEnv(headers, method);		   // [x] : generate env variable
	char tempFileName[] = "/tmp/.CgitempfileXXXXXXXX";
	int tempFD = -1;
	if (!body.empty())
	{
		tempFD = mkstemp(tempFileName);
		if (tempFD != -1)
		{
			write(tempFD, body.c_str(), body.length()), std::cout << "generate file: " << tempFileName << " fd:" << tempFD << std::endl;
			lseek(tempFD, 0, SEEK_SET);
		}
		else
		{
			this->responseStatus = "HTTP/1.1 500 Internal Server Error";
			std::string response = this->responseStatus + "\r\n"
														  "Content-Length: 21\r\n"
														  "Content-Type: text/plain\r\n"
														  "\r\n"
														  "Internal Server Error";
			send(clientSocket, response.c_str(), response.length(), 0);
			return;
		}
	}
	handleCGIScript(clientSocket, method, env, tempFD);
	// close(tempFD); // TODO: CGI DONE
	// unlink(tempFileName); // TODO: CGI DONE
}

void Response::parseUri(std::string uri)
{
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
			this->script_path = uri.substr(0, pos1);
		}
		else
			this->script_path = uri;
	}
	else
		this->script_path = uri;
	if (this->path_info.empty())
		this->path_info = "/";
	pos = this->script_path.find_last_of(".");
	if (pos != std::string::npos)
		this->extention = this->script_path.substr(pos + 1, this->script_path.length() - pos + 1);
	// std::cout << extention << std::endl;
	// std::cout << "query: " << this->query << std::endl;
	// std::cout << "path info: " << this->path_info << std::endl;
	// std::cout << "script: " << this->script_path << std::endl;
}

std::multimap<std::string, std::string> Response::parseHeader(int clientSocket, std::string buffer)
{
	std::multimap<std::string, std::string> headers;
	std::istringstream stream(buffer);
	std::string line;
	while (std::getline(stream, line))
	{
		if (line.empty() || line == "\r")
			break;
		size_t pos = line.find(':');
		if (pos == std::string::npos)
		{
			this->responseStatus = "HTTP/1.1 400 Bad Request";
			std::string response = this->responseStatus + "\r\n"
														  "Content-Length: 11\r\n"
														  "Content-Type: text/plain\r\n"
														  "\r\n"
														  "Bad Request";
			send(clientSocket, response.c_str(), response.length(), 0);
			break;
		}
		std::string key = line.substr(0, pos);
		std::string value = line.substr(pos + 2);
		headers.insert(std::make_pair(key, value));
	}
	printMap(headers);
	return headers;
}

std::multimap<std::string, std::string> Response::mergeHeadersValues(const std::multimap<std::string, std::string> &headers)
{
	std::multimap<std::string, std::string> newHeaders;
	for (std::multimap<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
	{
		const std::string &key = it->first;
		std::multimap<std::string, std::string>::iterator mergedIt = newHeaders.find(key);
		if (mergedIt == newHeaders.end())
			newHeaders.insert(std::make_pair(key, it->second));
		else
			mergedIt->second += ", " + it->second;
	}
	return newHeaders;
}

std::multimap<std::string, std::string> Response::generateCGIEnv(std::multimap<std::string, std::string> headers, std::string method)
{
	std::multimap<std::string, std::string>::iterator it;
	std::multimap<std::string, std::string>::iterator end;
	std::multimap<std::string, std::string> env;
	//*------------------------------*
	std::string SERVER_SOFTWARE = "webserv";   // MARK : value fix
	std::string SERVER_NAME = "localhost";	   // NOTE : my ip address
	std::string GATEWAY_INTERFACE = "CGI/1.1"; // MARK : value fix
	//*------------------------------*
	std::string SERVER_PROTOCOL = "HTTP/1.1"; // MARK : value fix
	std::string SERVER_PORT = "80";			  // NOTE : from config
	std::string REQUEST_METHOD = method;
	std::string PATH_INFO = this->path_info;
	std::string PATH_TRANSLATED = "/mnt/c/Users/SPARROW/Desktop/webserv/config/cgi-bin" + this->path_info;	 // NOTE : root + PATH_INFO
	std::string SCRIPT_NAME = "/mnt/c/Users/SPARROW/Desktop/webserv/config/cgi-bin" + this->script_path;	 // NOTE : root + script
	std::string SCRIPT_FILENAME = "/mnt/c/Users/SPARROW/Desktop/webserv/config/cgi-bin" + this->script_path; // NOTE : root +script
	// std::string PATH_TRANSLATED = "/Users/ael-mouz/Desktop/webserv/config/cgi-bin" + this->path_info;	// NOTE : root + PATH_INFO
	// std::string SCRIPT_NAME = "/Users/ael-mouz/Desktop/webserv/config/cgi-bin" + this->script_path;		// NOTE : root + script
	// std::string SCRIPT_FILENAME = "/Users/ael-mouz/Desktop/webserv/config/cgi-bin" + this->script_path; // NOTE : root +script
	std::string QUERY_STRING = this->query;
	std::string CONTENT_TYPE;
	it = headers.find("Content-Type");
	end = headers.end();
	if (it != end)
		CONTENT_TYPE = it->second;
	it = headers.find("Content-Length");
	end = headers.end();
	std::string CONTENT_LENGTH = "0";
	if (it != end)
		CONTENT_LENGTH = it->second;
	// std::string REMOTE_ADDR = "10.12.5.11"; // NOTE:client IP
	//*------------------------------*
	std::string REDIRECT_STATUS = "CGI"; // MARK: for php
	//*------------------------------*
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
	// env.insert(std::make_pair("REMOTE_ADDR", REMOTE_ADDR));
	env.insert(std::make_pair("REDIRECT_STATUS", REDIRECT_STATUS)); // NOTE:php only
	it = headers.begin();
	std::string keyEnv, valueEnv;
	for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
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
	printMap(env);
	return env;
}

void Response::handleCGIScript(int clientSocket, const std::string &method, std::multimap<std::string, std::string> env, int tempFD)
{
	int pipefd[2];
	if (pipe(pipefd) == -1)
	{
		this->responseStatus = "HTTP/1.1 500 Internal Server Error";
		std::string response = this->responseStatus + "\r\n"
													  "Content-Length: 21\r\n"
													  "Content-Type: text/plain\r\n"
													  "\r\n"
													  "Internal Server Error";
		send(clientSocket, response.c_str(), response.length(), 0);
		return;
	}
	pid_t pid = fork();
	if (pid == -1)
	{
		this->responseStatus = "HTTP/1.1 500 Internal Server Error";
		std::string response = this->responseStatus + "\r\n"
													  "Content-Length: 21\r\n"
													  "Content-Type: text/plain\r\n"
													  "\r\n"
													  "Internal Server Error";
		send(clientSocket, response.c_str(), response.length(), 0);
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
		// alarm(1);
		if (tempFD != -1 && method == "POST")
		{
			dup2(tempFD, STDIN_FILENO);
			close(tempFD);
		}
		if (this->extention == "py")
		{
			char *const args[] = {(char *)"python3", (char *)env.find("SCRIPT_FILENAME")->second.c_str(), NULL}; // NOTE: for pyhton
			execve("/usr/bin/python3", args, envp);
		}
		else if (this->extention == "pl")
		{
			char *const args[] = {(char *)"perl", (char *)env.find("SCRIPT_FILENAME")->second.c_str(), NULL}; // NOTE: for pyhton
			execve("/usr/bin/perl", args, envp);
		}
		else if (this->extention == "php")
		{
			std::cout  << "helloo" << std::endl;
			char *const args[] = {(char *)"php-cgi_bin", (char *)env.find("SCRIPT_FILENAME")->second.c_str(), NULL}; // NOTE: for pyhton
			execve("/mnt/c/Users/SPARROW/Desktop/webserv/tests/php-cgi_bin", args, envp);
		}
		perror("execve failed");
		exit(EXIT_FAILURE);
	}
	else
	{
		close(pipefd[1]);
		int status;
		waitpid(pid, &status, 0);
		if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
		{
			char buffer[4096];
			ssize_t bytesRead;
			while ((bytesRead = read(pipefd[0], buffer, 4096)) > 0)
			{
				std::cout << buffer << std::endl;
				send(clientSocket, buffer, bytesRead, 0);
			}
		}
		else
		{
			this->responseStatus = "HTTP/1.1 502 Bad Gateway";
			std::string response = this->responseStatus + "\r\n"
														  "Content-Length: 11\r\n"
														  "Content-Type: text/plain\r\n"
														  "\r\n"
														  "Bad Gateway";
			send(clientSocket, response.c_str(), response.length(), 0);
			return;
		}
		close(pipefd[0]);
	}
}
