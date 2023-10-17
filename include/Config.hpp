/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mouz <ael-mouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/12 14:59:47 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/17 16:08:03 by ael-mouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <string>
#include "ServerConfig.hpp"

class Config
{
private:
	int NbServer;
	std::vector<ServerConfig> Serverconfig;

public:
	~Config(void);
	Config(std::string filename);
	void parseServer(const std::string &data, ServerConfig &serverConfig, int start, std::string filename);
	void parseRoute(const std::string &data, Route &route, int start, std::string filename);
	void printConfig(void);
};

std::string trim(const std::string &str, const std::string &charactersToTrim);