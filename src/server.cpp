/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mouz <ael-mouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/11 20:31:55 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/30 12:02:31 by ael-mouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Response.hpp"
#include "../include/ServerConfig.hpp"

const int PORT = 8080;
const int BACKLOG = 10;

void parseRequest(const std::string &request, std::string &method, std::string &uri, std::string &httpVersion, std::string &headers, std::string &body)
{
    std::istringstream requestStream(request);
    std::string requestLine;
    std::getline(requestStream, requestLine);
    std::vector<std::string> requestParts = splitString(requestLine, " ");
    if (requestParts.size() >= 3)
    {
        method = requestParts[0];
        uri = requestParts[1];
        httpVersion = requestParts[2];
    }
    std::string line;
    while (std::getline(requestStream, line))
    {
        if (line.empty() || line == "\r")
            break;
        headers += line + "\n";
    }
    size_t pos = request.find("\r\n\r\n");
    if (pos != std::string::npos)
        body = request.substr(pos + 4, request.length() - pos + 4);
}

std::string receiveRequest(int clientSocket)
{
    char buffer[1024];
    std::string request;
    std::string contentLengthHeader = "Content-Length: ";
    std::size_t contentLength = 0;
    bool headersComplete = false;
    std::size_t pos = std::string::npos;
    while (true)
    {
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0)
        {
            std::cerr << "Error reading request" << std::endl;
            break;
        }
        request.append(buffer, bytesRead);
        if (!headersComplete)
        {
            pos = request.find("\r\n\r\n");
            if (pos != std::string::npos)
            {
                headersComplete = true;
                std::string headers = request.substr(0, pos);
                std::size_t contentLengthPos = headers.find(contentLengthHeader);
                if (contentLengthPos != std::string::npos)
                {
                    std::size_t headerEndPos = headers.find("\r\n", contentLengthPos);
                    const std::string contentLengthStr = headers.substr(contentLengthPos + contentLengthHeader.length(), headerEndPos - (contentLengthPos + contentLengthHeader.length()));
                    try
                    {
                        contentLength = std::stoul(contentLengthStr);
                    }
                    catch (const std::exception &e)
                    {
                        std::cerr << "Error parsing Content-Length: " << e.what() << std::endl;
                        break;
                    }
                }
            }
        }
        if (headersComplete && request.size() >= pos + 4 + contentLength)
            break;
    }
    return request;
}

void StartSrever(const ServerConfig &conf)
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
    // int optval = 1;
    // if (setsockopt(clientSocket, SOL_SOCKET, SO_NOSIGPIPE, &optval, sizeof(optval)) < 0)
    //     throw std::runtime_error("Error: Failed to setsockopt for SO_NOSIGPIPE");
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
        logMessage(ERROR, "Error binding"), exit(1);
    if (listen(serverSocket, BACKLOG) == -1)
        logMessage(ERROR, "Error listening"), exit(1);
    logMessage(INFO, "Server listening on http://localhost:8080");
    while (true)
    {
        if ((clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen)) == -1)
        {
            logMessage(ERROR, "Error accepting connection");
            continue;
        }
        std::string clientIP = inet_ntoa(clientAddr.sin_addr);
        logMessage(INFO, "Received connection from " + clientIP);

        std::string request = receiveRequest(clientSocket);
        std::string method, uri, httpVersion, Rheaders, body;
        parseRequest(request, method, uri, httpVersion, Rheaders, body);
        // std::cout << std::setfill('-') << std::setw(50) << "-" << std::endl;
        // std::cout << "Method: " << method << std::endl;
        // std::cout << std::setfill('-') << std::setw(50) << "-" << std::endl;
        // std::cout << "URI: " << uri << std::endl;
        // std::cout << std::setfill('-') << std::setw(50) << "-" << std::endl;
        // std::cout << "HTTP Version: " << httpVersion << std::endl;
        // std::cout << std::setfill('-') << std::setw(50) << "-" << std::endl;
        // std::cout << "Headers:\n" + convertText(Rheaders) << std::endl;
        // std::cout << std::setfill('-') << std::setw(50) << "-" << std::endl;
        // std::cout << "Body:\nbodysize:"
        // 		  << body.length() << "\n" + convertText(body) << std::endl;
        // std::cout << std::setfill('-') << std::setw(50) << "-" << std::endl;
        // std::cout << convertText(request) << std::endl;
        Response s;
        s.response(clientSocket, method, uri, httpVersion, Rheaders, body, conf);
        close(clientSocket);
    }
    close(serverSocket);
}
