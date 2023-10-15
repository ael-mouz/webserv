/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mouz <ael-mouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/11 20:31:55 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/15 16:58:54 by ael-mouz         ###   ########.fr       */
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
	if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) // reuse the same port without probleme
		throw std::runtime_error("Error: Failed to setsockopt for reuse");
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
	{
		logMessage(ERROR, "Error binding");
		exit(1);
	}
	if (listen(serverSocket, BACKLOG) == -1)
	{
		logMessage(ERROR, "Error listening");
		exit(1);
	}
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

		// char buffer[4096]; // Adjust the buffer size as needed
		// int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
		// if (bytesRead == -1)
		// {
		// 	std::cerr << "Error reading from socket" << std::endl;
		// 	close(clientSocket);
		// 	continue;
		// }
		// buffer[bytesRead] = '\0';
		// std::cout << buffer << std::endl;

		char buffer[4096];
		std::string request;
		bool hasContentLength = false;
		int contentLength = 0;

		while (true)
		{
			int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
			if (bytesRead == -1)
			{
				std::cerr << "Error reading from socket" << std::endl;
				close(clientSocket);
				break;
			}
			if (bytesRead == 0)
				break;
			buffer[bytesRead] = '\0';
			request += buffer;
			if (!hasContentLength)
			{
				size_t pos = request.find("Content-Length: ");
				if (pos != std::string::npos)
				{
					hasContentLength = true;
					contentLength = std::stoi(request.substr(pos + 15));
				}
			}
			if (hasContentLength && request.length() - request.find("\r\n\r\n") - 4 >= (size_t)contentLength)
				break;
		}
		std::string requestBody;
		if (hasContentLength)
			requestBody = request.substr(request.find("\r\n\r\n") + 4);
		// parse request
		std::cout << request << std::endl;
		std::cout << convertText(request) << std::endl;
		std::multimap<std::string, std::string> headers;
		std::string line;
		// std::istringstream stream(buffer);
		std::istringstream stream(request);
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

		// std::cout << "body :\n" << convertText(requestBody) << std::endl;
		// char *const env[] = {
		// 	// (char *)"CONTENT_TYPE=multipart/form-data; boundary=--------------------------157068096586764982468991",
		// 	// (char *)"CONTENT_LENGTH=831",
		// 	(char *)"HTTP_COOKIE=name=ayman; token=pQB5xyNZnVCWUE6g",
		// 	// (char *)"HTTP_USER_AGENT=PostmanRuntime/7.33.0",
		// 	// (char *)"PATH_INFO=/path/to/cgi_script",
		// 	(char *)"QUERY_STRING=",
		// 	// (char *)"REMOTE_ADDR=client_IP_address",
		// 	// (char *)"REMOTE_HOST=client_hostname",
		// 	// (char *)"REQUEST_METHOD=POST",
		// 	// (char *)"SCRIPT_FILENAME=/path/to/cgi_script",
		// 	// (char *)"SCRIPT_NAME=cgi_script_name",
		// 	// (char *)"SERVER_NAME=your_server_name_or_IP",
		// 	// (char *)"SERVER_SOFTWARE=YourServerSoftware/1.0",
		// 	NULL};
		// char *const env[] = {
		// 	strdup(("CONTENT_TYPE=" + headers.find("Content-Type")->second).c_str()),
		// 	strdup(("CONTENT_LENGTH=" + headers.find("Content-Length")->second).c_str()),
		// 	strdup(("HTTP_COOKIE=" + headers.find("Cookie")->second).c_str()),
		// 	NULL};
		// // Create a pipe for communication
		// int pipefd[2];
		// if (pipe(pipefd) == -1)
		// {
		// 	std::cerr << "Error creating pipe" << std::endl;
		// 	return 1;
		// }

		// pid_t pid = fork();
		// if (pid == -1)
		// {
		// 	std::cerr << "Error forking" << std::endl;
		// 	return 1;
		// }
		// else if (pid == 0)
		// {						 // Child process
		// 	close(clientSocket); // Close the server socket in the child process
		// 	close(pipefd[1]);	 // Close the write end of the pipe

		// 	// Redirect the read end of the pipe to stdin
		// 	dup2(pipefd[0], STDIN_FILENO);
		// 	close(pipefd[0]); // Close the read end of the pipe

		// 	alarm(5);
		// 	const char *perl_script = "/Users/ael-mouz/Desktop/webserv/www/cgi_test_file/upload.pl"; // Replace with the actual path to your Perl script
		// 	char *const args[] = {(char *)"perl", (char *)perl_script, NULL};
		// 	execve("/usr/bin/perl", args, env);
		// 	std::cerr << "Error executing Perl script" << std::endl;
		// 	exit(1);
		// }
		// else
		// { // Parent process
		// 	close(clientSocket);
		// 	close(pipefd[0]); // Close the read end of the pipe

		// 	// Write the request body to the write end of the pipe
		// 	write(pipefd[1], requestBody.c_str(), requestBody.size());
		// 	close(pipefd[1]); // Close the write end of the pipe
		// }
		// pid_t pid = fork();
		// if (pid == -1)
		// {
		// 	logMessage(ERROR, "Error forking");
		// 	close(clientSocket);
		// 	continue;
		// }
		// else if (pid == 0)
		// {
		// 	close(serverSocket); // Close the server socket in the child process
		// 	dup2(clientSocket, STDOUT_FILENO);
		// 	dup2(clientSocket, STDOUT_FILENO);
		// 	close(clientSocket); // Close the client socket in the child process
		// 	// alarm(5);
		// 	const char *perl_script = "/Users/ael-mouz/Desktop/webserv/www/cgi_test_file/index_cookie.pl";
		// 	// const char *perl_script = "/Users/ael-mouz/Desktop/webserv/www/cgi_test_file/env.pl";
		// 	char *const args[] = {(char *)"perl", (char *)perl_script, NULL};
		// 	execve("/usr/bin/perl", args, env);
		// 	// execve("/usr/bin/perl", args, NULL);
		// 	logMessage(ERROR, "Error executing Perl script");
		// 	exit(1);
		// }
		// else
		// {
		// 	close(clientSocket);
		// }

		// pid_t pid = fork();
		// if (pid == -1)
		// {
		// 	logMessage(ERROR, "Error forking");
		// 	close(clientSocket);
		// }
		// else if (pid == 0)
		// {
		// 	dup2(clientSocket, STDOUT_FILENO);
		// 	close(clientSocket);
		// 	const char *python_script = "/Users/ael-mouz/Desktop/webserv/www/cgi_test_file/index.py";
		// 	char *const args[] = {(char *)"python3", (char *)python_script, NULL};
		// 	execvp("python3", args);
		// 	logMessage(ERROR, "Error executing Python script");
		// 	exit(1);
		// }
		// else
		// {
		// 	// Parent process
		// 	close(clientSocket);
		// }
	}
	close(serverSocket);
	return 0;
}
