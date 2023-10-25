/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mouz <ael-mouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/12 14:59:47 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/25 20:33:53 by ael-mouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Include.hpp"
#include "ServerConfig.hpp"

class Config
{
private:
    int NbServer;
    std::vector<ServerConfig> Serverconfig;

public:
    Config(void);
    ~Config(void);
    void parser(std::string filename);
    void parseServer(const std::string &data, ServerConfig &serverConfig, int start, const std::string &filename);
    void parseRoute(const std::string &data, Route &route, int start, const std::string &filename);
    void parseBodySize(std::string &sizeStr, int start, const std::string &line, const std::string &filename);
    void printConfig(void);
    /*checker*/
    void parseMethods(Route &route, int start, const std::string &line, const std::string &filename);
    void parsePort(std::string &port, int start, const std::string &line, const std::string &filename);
    void parseRedirection(Route &route, int start, const std::string &line, const std::string &filename);
    void parsePath(const std::string &path, int start, const std::string &line, const std::string &filename);
    void parseRoutePath(const std::string &path, int start, const std::string &line, const std::string &filename);
    void parseAutoindex(const std::string &value, int start, const std::string &line, const std::string &filename);
    void parseServerNameAndHostName(const std::string &serverName, int start, const std::string &line, const std::string &filename);
    /*getter and setter*/
    int getNbServer(void) const;
    ServerConfig getServerConfig(size_t index) const;
};
