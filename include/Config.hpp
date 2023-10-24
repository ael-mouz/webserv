/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mouz <ael-mouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/12 14:59:47 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/24 18:48:42 by ael-mouz         ###   ########.fr       */
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
    void parseServer(const std::string &data, ServerConfig &serverConfig, int start,const std::string &filename);
    void parseRoute(const std::string &data, Route &route, int start,const std::string &filename);
    void parseBodySize(std::string &sizeStr, int start, const std::string &line, const std::string &filename);
    void printConfig(void);
    /*getter and setter*/
    int getNbServer(void) const;
    ServerConfig getServerConfig(size_t index) const;
};
