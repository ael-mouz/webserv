/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mouz <ael-mouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/18 17:01:10 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/23 13:18:25 by ael-mouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Include.hpp"
#include "ServerConfig.hpp"
class Response
{
private:
    std::string responseStatus;
    std::string query;
    std::string path_info;
    std::string script_path;
    std::string extention;

public:
    void response(int clientSocket, std::string method, std::string uri, std::string httpVersion, std::string Rheaders, std::string body, const ServerConfig &conf);
    void parseUri(std::string uri);
    std::multimap<std::string, std::string> parseHeader(int clientSocket, std::string buffer, const ServerConfig &conf);
    std::multimap<std::string, std::string> mergeHeadersValues(const std::multimap<std::string, std::string> &headers);
    std::multimap<std::string, std::string> generateCGIEnv(std::multimap<std::string, std::string> headers, std::string method);
    void handleCGIScript(int clientSocket, const std::string &method, std::multimap<std::string, std::string> env, int tempFD, const ServerConfig &conf);
    void generateResponse(std::string status, const ServerConfig &conf);
};
