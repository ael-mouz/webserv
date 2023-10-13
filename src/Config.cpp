/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mouz <ael-mouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/13 19:59:44 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/13 21:55:58 by ael-mouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Config.hpp"

std::string Config::ParseServer(std::ifstream &infile)
{
	std::string data;
	std::string line;
	while (std::getline(infile, line) && line != "Server:" && this->Error != 1)
	{
		if (!line.empty())
		{
			std::cerr << "\e[41mError\e[49m" << std::endl;
			this->Error = 1;
		}
	}
	while (std::getline(infile, line) && line != "EndServer;" && this->Error != 1)
	{
		if (!line.empty() && line[0] != '\t')
		{
			std::cerr << "\e[41mError\e[49m" << std::endl;
			this->Error = 1;
		}
		if (!line.empty())
		{
			line.erase(0, 1);
			data += line;
			data += "\n";
		}
	}
	return data;
};

int Config::countNbServer(std::string filename)
{
	std::ifstream infile;
	std::string data;
	std::string line;
	int count;

	infile.open(filename.c_str());
	if (!infile.is_open())
	{
		std::cerr << "Error openning file " << filename << std::endl;
		this->Error = 1;
	}
	count = 0;
	while (std::getline(infile, line) && this->Error != 1)
		if (line == "Server:")
			count++;
	infile.close();
	return count;
};

Config::Config(std::string filename) : Error(0), NbServer(0)
{
	std::ifstream infile;
	infile.open(filename.c_str());
	if (!infile.is_open())
	{
		std::cerr << "Error openning file " << filename << std::endl;
		this->Error = 1;
	}
	this->NbServer = countNbServer(filename);
	(void) this->Serverconf;
	// this->Serverconf = new ServerConf[this->NbServer];
	std::cout << this->NbServer << std::endl;
	for(int i = 0; i < this->NbServer;i++)
	{
		std::cout <<"---------------------------"<< std::endl;
		std::string data;
		data = ParseServer(infile);
		std::cout << data << std::endl;
		std::cout <<"---------------------------"<< std::endl;
		// this->Serverconf[i].setData(data);
	}
	infile.close();
}

Config::~Config(void)
{
}
// bool Config::getEroor(void)
// {
// }
// int Config::getNbServer(void)
// {
// }
// ServerConf Config::getServerconf(int index)
// {
// 	(void)index;
// }