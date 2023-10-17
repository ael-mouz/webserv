/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mouz <ael-mouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/12 14:59:02 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/17 22:20:07 by ael-mouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/server.hpp"

void handleClient(int clientSocket)
{
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
	// 		return;
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
	// std::cout << convertText(requestBody) << std::endl;
	// // Parse the request method (e.g., GET, POST, DELETE)
	// std::istringstream stream(request);
	// std::string method;
	// std::getline(stream, method);

	// // Extract headers and process them if necessary
	// std::multimap<std::string, std::string> headers;
	// std::string line;
	// while (std::getline(stream, line))
	// {
	// 	if (line.empty() || line == "\r")
	// 		break;
	// 	size_t pos = line.find(':');
	// 	if (pos == std::string::npos)
	// 		break;
	// 	std::string key = line.substr(0, pos);
	// 	std::string value = line.substr(pos + 2);
	// 	headers.insert(std::make_pair(key, value));
	// }

	// // Execute Python CGI script for POST requests
	// if (method.find("POST") != std::string::npos)
	// {
	// 	// You can execute the Python script here and pass requestBody as input
	// 	// Example:
	// 	// const char *python_script = "/path/to/your/python/script.py";
	// 	// char *const args[] = {(char *)"python3", (char *)python_script, NULL};
	// 	// execve("python3", args, env);

	// 	// Handle response from the Python script (stdout) and send it back to the client
	// 	// Ensure proper error handling and response generation
	// }

	close(clientSocket);
}
