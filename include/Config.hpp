/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mouz <ael-mouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/12 14:59:47 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/23 13:17:43 by ael-mouz         ###   ########.fr       */
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
	Config(std::string filename);
	~Config(void);
	void parseServer(const std::string &data, ServerConfig &serverConfig, int start, std::string filename);
	void parseRoute(const std::string &data, Route &route, int start, std::string filename);
	void printConfig(void);
    ServerConfig getServerConfig(size_t index) const;
};
