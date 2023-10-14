/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mouz <ael-mouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/12 14:59:47 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/14 22:33:36 by ael-mouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include "ServerConfig.hpp"

class Config
{
private:
	bool Error;
	int NbServer;
	std::vector<ServerConfig> Serverconfig;

public:
	Config(std::string filename);
	~Config(void);
	void ParseServer(std::string data);
	void countNbServer(std::string filename);
	std::string SpiltServer(std::ifstream &infile);
};

std::string trim(const std::string &str, const std::string &charactersToTrim);