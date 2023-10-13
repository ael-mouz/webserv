/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mouz <ael-mouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/12 14:59:47 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/13 21:56:13 by ael-mouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <fstream>
#include "ServerConf.hpp"

class Config
{
private:
	bool Error;
	int NbServer;
	ServerConf *Serverconf;

public:
	Config(std::string filename);
	~Config(void);
	int countNbServer(std::string filename);
	std::string ParseServer(std::ifstream &infile);
	// bool getEroor(void);
	// int getNbServer(void);
	// ServerConf getServerconf(int index);
};
