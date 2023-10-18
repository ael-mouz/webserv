/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mouz <ael-mouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/11 20:31:55 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/18 17:04:48 by ael-mouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/server.hpp"

const int PORT = 8080;
const int BACKLOG = 10;

int main()
{
	int serverSocket, clientSocket;
	struct sockaddr_in serverAddr, clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		logMessage(ERROR, "Error creating socket");
		exit(1);
	}
	int reuse = 1;
	if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
		throw std::runtime_error("Error: Failed to setsockopt for reuse");
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
		logMessage(ERROR, "Error binding"), exit(1);
	if (listen(serverSocket, BACKLOG) == -1)
		logMessage(ERROR, "Error listening"), exit(1);
	logMessage(INFO, "Server listening on http://localhost:" + std::to_string(PORT));
	while (true)
	{
		if ((clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen)) == -1)
		{
			logMessage(ERROR, "Error accepting connection");
			continue;
		}
		std::string clientIP = inet_ntoa(clientAddr.sin_addr);
		logMessage(INFO, "Received connection from " + clientIP);

		char buffer[4096]; // Adjust the buffer size as needed
		int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
		if (bytesRead == -1)
		{
			std::cerr << "Error reading from socket" << std::endl;
			close(clientSocket);
			continue;
		}
		else if (bytesRead == 0)
			continue;
		buffer[bytesRead] = '\0';
		std::cout << buffer << std::endl;

		// char buffer[4096];
		// std::string request;
		// bool hasContentLength = false;
		// int contentLength = 0;

		// while (true)
		// {
		// 	int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
		// 	if (bytesRead == -1)
		// 	{
		// 		std::cerr << "Error reading from socket" << std::endl;
		// 		close(clientSocket);
		// 		break;
		// 	}
		// 	if (bytesRead == 0)
		// 		break;
		// 	buffer[bytesRead] = '\0';
		// 	request += buffer;
		// 	if (!hasContentLength)
		// 	{
		// 		size_t pos = request.find("Content-Length: ");
		// 		if (pos != std::string::npos)
		// 		{
		// 			hasContentLength = true;
		// 			contentLength = std::stoi(request.substr(pos + 15));
		// 		}
		// 	}
		// 	if (hasContentLength && request.length() - request.find("\r\n\r\n") - 4 >= (size_t)contentLength)
		// 		break;
		// }
		// std::string requestBody;
		// if (hasContentLength)
		// 	requestBody = request.substr(request.find("\r\n\r\n") + 4);
		// parse request
		// std::cout << request << std::endl;
		// std::cout << convertText(request) << std::endl;
		// std::istringstream stream(request);
		std::multimap<std::string, std::string> headers;
		std::string line;
		std::istringstream stream(buffer);
		std::string method;
		std::getline(stream, method);
		std::cout << "method : " << method << std::endl;
		while (std::getline(stream, line))
		{
			if (line.empty() || line == "\r")
				break;
			size_t pos = line.find(':');
			if (pos == std::string::npos)
				break;
			std::string key = line.substr(0, pos);
			std::string value = line.substr(pos + 2);
			headers.insert(std::make_pair(key, value));
		}
		std::cout << std::setfill('-') << std::setw(30) << "\n"
				  << std::endl;
		std::multimap<std::string, std::string>::const_iterator it = headers.begin();
		for (; it != headers.end(); ++it)
			std::cout << "Key: " << std::setfill(' ') << std::setw(30) << it->first << ", Value: " << std::setw(100) << it->second << std::endl;
		std::cout << std::setfill('-') << std::setw(30) << "\n"
				  << std::endl;

		// std::cout << "body :\n"
		// 		  << convertText(requestBody) << std::endl;
		char *const env[] = {
			(char *)("PATH_INFO="),
			(char *)("REQUEST_METHOD="),
			(char *)("SCRIPT_FILENAME=/Users/ael-mouz/Desktop/webserv/config/cgi-bin/python/upload_multi_files.py"),
			NULL};

		// Set other environment variables
		std::string content_type_str = "CONTENT_TYPE=" + headers.find("Content-Type")->second;
		std::string content_length_str = "CONTENT_LENGTH=" + headers.find("Content-Length")->second;
		std::string http_user_agent_str = "HTTP_USER_AGENT=" + headers.find("User-Agent")->second;
		std::string http_host_str = "HTTP_HOST=" + headers.find("Host")->second;
		std::string http_accept_str = "HTTP_ACCEPT=" + headers.find("Accept")->second;
		std::string http_postman_token_str = "HTTP_POSTMAN_TOKEN=" + headers.find("Postman-Token")->second;
		std::string http_accept_encoding_str = "HTTP_ACCEPT_ENCODING=" + headers.find("Accept-Encoding")->second;
		std::string http_connection_str = "HTTP_CONNECTION=" + headers.find("Connection")->second;
		std::string http_cookie_str = "HTTP_COOKIE=" + headers.find("Cookie")->second;

		// Set the environment variables

		pid_t pid = fork();
		if (pid == -1)
		{
			logMessage(ERROR, "Error forking");
			close(clientSocket);
		}
		else if (pid == 0)
		{
			setenv("CONTENT_TYPE", content_type_str.c_str(), 1);
			setenv("CONTENT_LENGTH", content_length_str.c_str(), 1);
			setenv("HTTP_USER_AGENT", http_user_agent_str.c_str(), 1);
			setenv("HTTP_HOST", http_host_str.c_str(), 1);
			setenv("HTTP_ACCEPT", http_accept_str.c_str(), 1);
			setenv("HTTP_POSTMAN_TOKEN", http_postman_token_str.c_str(), 1);
			setenv("HTTP_ACCEPT_ENCODING", http_accept_encoding_str.c_str(), 1);
			setenv("HTTP_CONNECTION", http_connection_str.c_str(), 1);
			setenv("HTTP_COOKIE", http_cookie_str.c_str(), 1);
			dup2(clientSocket, STDOUT_FILENO);
			close(clientSocket);
			const char *python_script = "/Users/ael-mouz/Desktop/webserv/config/cgi-bin/python/upload_multi_files.py";
			char *const args[] = {(char *)"python3", (char *)python_script, NULL};
			execve("python3", args, env);
			logMessage(ERROR, "Error executing Python script");
			exit(1);
		}
		else
		{

			char buffer[4096]; // Adjust the buffer size as needed
			int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
			if (bytesRead == -1)
			{
				std::cerr << "Error reading from socket" << std::endl;
				close(clientSocket);
				continue;
			}
			else if (bytesRead == 0)
				continue;
			buffer[bytesRead] = '\0';
			std::cout << buffer << std::endl;
			close(clientSocket);
		}

		// perl
		//  pid_t pid = fork();
		//  if (pid == -1)
		//  {
		//  	logMessage(ERROR, "Error forking");
		//  	close(clientSocket);
		//  	continue;
		//  }
		//  else if (pid == 0)
		//  {
		//  	close(serverSocket);
		//  	dup2(clientSocket, STDOUT_FILENO);
		//  	close(clientSocket);
		//  	const char *perl_script = "/Users/ael-mouz/Desktop/webserv/www/cgi_test_file/index_cookie.pl";
		//  	// const char *perl_script = "/Users/ael-mouz/Desktop/webserv/www/cgi_test_file/env.pl";
		//  	char *const args[] = {(char *)"perl", (char *)perl_script, NULL};
		//  	execve("/usr/bin/perl", args, env);
		//  	// execve("/usr/bin/perl", args, NULL);
		//  	logMessage(ERROR, "Error executing Perl script");
		//  	exit(1);
		//  }
		//  else
		//  {
		//  	close(clientSocket);
		//  }
	}
	close(serverSocket);
	return 0;
}
