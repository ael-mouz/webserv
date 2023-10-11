/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mouz <ael-mouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/11 20:31:55 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/11 23:21:38 by ael-mouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/server.hpp"

const int PORT = 8089;
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

		char buffer[4096]; // Adjust the buffer size as needed
		int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
		if (bytesRead == -1)
		{
			std::cerr << "Error reading from socket" << std::endl;
			close(clientSocket);
			continue;
		}
		buffer[bytesRead] = '\0';
		std::cout << buffer << std::endl;
		pid_t pid = fork();
		if (pid == -1)
		{
			logMessage(ERROR, "Error forking");
			close(clientSocket);
			continue;
		}
		else if (pid == 0)
		{
			// dup2(clientSocket, STDOUT_FILENO);
			close(serverSocket); // Close the server socket in the child process
			dup2(clientSocket, STDOUT_FILENO);
			close(clientSocket); // Close the client socket in the child process
			const char *perl_script = "/Users/ael-mouz/Desktop/webserv/www/cgi_test_file/index_cookie.pl";
			char *const args[] = {(char *)"perl", (char *)perl_script, NULL};
			execve("/usr/bin/perl", args, NULL);
			logMessage(ERROR, "Error executing Perl script");
			exit(1);
		}
		else
		{
			close(clientSocket);
		}
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
