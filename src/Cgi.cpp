/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mouz <ael-mouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/18 16:57:43 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/18 22:46:39 by ael-mouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Cgi.hpp"

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

std::string convertText(std::string a)
{
	std::string::size_type i = 0;
	std::string newbuff;
	while (i < a.length())
	{
		if (a[i] == '\r' && i + 1 < a.length() && a[i + 1] == '\n')
		{
			newbuff += "\e[41m\\r\\n\e[49m\n";
			i++;
		}
		else if (a[i] == '\t')
			newbuff += "\\t\t";
		else if (a[i] == '\v')
			newbuff += "\\v\v";
		else if (a[i] == '\f')
			newbuff += "\\f\f";
		else
			newbuff += a[i];
		i++;
	}
	return newbuff;
}
void Cgi::executeCgi()
{
	std::string buffer =
		"User-Agent: PostmanRuntime / 7.33.0 \r\n"
		"Accept: */*\r\n"
		"Postman-Token: 2dd4127a-2d96-4485-87f2-f862db89ec3a\r\n"
		"Host: localhost:8080\r\n"
		"Accept-Encoding: gzip, deflate, br\r\n"
		"Connection: keep-alive\r\n"
		"Cookie: name=ayman; token=pQB5xyNZnVCWUE6g\r\n\r\n";
	// std::cout << convertText(buffer) << std::endl;
	// parse header and uri
	std::string method = "GET";
	std::string uri = "/www/index.html/about?name=ayman";
	std::string httpversion = "HTTP/1.1";
	parseUri(uri);
	if (method != "GET" && method != "POST" && method != "DELETE")
		this->responseStatus = "HTTP/1.1 501 Not Implemented";
	else if (httpversion != "HTTP/1.1")
		this->responseStatus = "HTTP/1.1 505 HTTP Version Not Supported";
	std::multimap<std::string, std::string> headers = parseHeader(buffer);
	// generate env variable
	//  	std::multimap<std::string, std::string>
	//  		envVAR;
	//  headers.insert(std::make_pair(key, value));
	//  if (setenv("MY_VARIABLE", "Hello, World!", 1) != 0) {
	//      perror("setenv");
	//      return 1;
	//  }
	//*------------------------------*
	std::string SERVER_SOFTWARE = "webserv";
	std::string SERVER_NAME= "localhost"; // MARK: my ip address
	std::string GATEWAY_INTERFACE = "CGI/1.1";
	//*------------------------------*
	std::string SERVER_PROTOCOL="HTTP/1.1";
	std::string SERVER_PORT="80";
	std::string REQUEST_METHOD = "POST";
	std::string PATH_INFO = "/about";
	std::string PATH_TRANSLATED="/var/www/html/about"; // NOTE: root + PATH_INFO
	std::string SCRIPT_NAME="/Users/ael-mouz/Desktop/webserv/config/cgi_test_file/upload5.py";
	std::string SCRIPT_FILENAME = "/Users/ael-mouz/Desktop/webserv/config/cgi_test_file/upload5.py";
	std::string QUERY_STRING = "name=John&email=john@example.com";
	std::string CONTENT_TYPE = "multipart/form-data; boundary=--------------------------163772461237889740355993";
	std::string CONTENT_LENGTH = "1454";
	std::string REMOTE_ADDR= "10.12.5.11"; // NOTE:client IP
	//*------------------------------*
	std::string REDIRECT_STATUS = "CGI"; // MARK: for php
}

void Cgi::parseUri(std::string uri)
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
	// std::cout << "query: " << this->query << std::endl;
	// std::cout << "path info: " << this->path_info << std::endl;
	// std::cout << "script: " << this->script_path << std::endl;
}

std::multimap<std::string, std::string> Cgi::parseHeader(std::string buffer)
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
			break;
		}
		std::string key = line.substr(0, pos);
		std::string value = line.substr(pos + 2);
		headers.insert(std::make_pair(key, value));
	}
	std::cout << std::setfill('-') << std::setw(150) << "-" << std::endl;
	std::multimap<std::string, std::string>::const_iterator it = headers.begin();
	for (; it != headers.end(); ++it)
		std::cout << "Key: " << std::setfill(' ') << std::setw(30) << it->first << "| Value: " << std::setw(100) << it->second << std::endl;
	std::cout << std::setfill('-') << std::setw(150) << "-" << std::endl;
	return headers;
}

// #include <iostream>
// #include <map>
// #include <vector>
// #include <string>
// #include <algorithm>

// void generateCGIEnvironmentVariables(const std::map<std::string, std::string>& headers, std::vector<std::string>& envVars) {
//     // Define the CGI environment variables.
//     std::string serverSoftware = "MyWebServer/1.0"; // Customize this with your server's info.
//     std::string gatewayInterface = "CGI/1.1"; // Adjust as needed.

//     // Add predefined CGI environment variables to the vector.
//     envVars.push_back("SERVER_SOFTWARE=" + serverSoftware);
//     envVars.push_back("GATEWAY_INTERFACE=" + gatewayInterface);

//     // Iterate through the HTTP headers to create additional CGI environment variables.
//     for (const auto& header : headers) {
//         std::string key = "HTTP_" + header.first;
//         std::transform(key.begin(), key.end(), key.begin(), ::toupper);
//         std::replace(key.begin(), key.end(), '-', '_');
//         std::string value = header.second;

//         envVars.push_back(key + "=" + value);
//     }
// }

// int main() {
//     std::map<std::string, std::string> headers = {
//         {"Host", "localhost:80"},
//         {"Connection", "keep-alive"},
//         {"Cache-Control", "max-age=0"},
//         {"DNT", "1"},
//         {"Upgrade-Insecure-Requests", "1"},
//         {"User-Agent", "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/118.0.0.0 Safari/537.36"},
//         {"Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7"},
//         {"Accept-Encoding", "gzip, deflate"},
//         {"Accept-Language", "en-US,en;q=0.9,fr;q=0.8,ar;q=0.7"},
//         {"sec-fetch-dest", "document"}
//     };

//     std::vector<std::string> envVars;
//     generateCGIEnvironmentVariables(headers, envVars);

//     // Print the generated CGI environment variables.
//     for (const std::string& envVar : envVars) {
//         std::cout << envVar << std::endl;
//     }

//     return 0;
// }
