/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mouz <ael-mouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/13 19:59:44 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/14 22:24:15 by ael-mouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Config.hpp"

std::string Config::SpiltServer(std::ifstream &infile)
{
	std::string data;
	std::string line;
	while (std::getline(infile, line) && line != "Server;" && this->Error != 1)
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
	if (line != "EndServer;")
	{
		std::cerr << "\e[41mError\e[49m" << std::endl;
		this->Error = 1;
	}
	return data;
};

void Config::countNbServer(std::string filename)
{
	std::ifstream infile;
	std::string data;
	std::string line;

	infile.open(filename.c_str());
	if (!infile.is_open())
	{
		std::cerr << "Error openning file " << filename << std::endl;
		this->Error = 1;
	}
	while (std::getline(infile, line) && this->Error != 1)
	{
		if (line == "EndServer;")
		{
			std::cerr << "\e[41mError srever not closed\e[49m" << std::endl;
			this->Error = 1;
			this->NbServer = -1;
			break;
		}
		else if (line == "Server;")
		{
			while (std::getline(infile, line) && line != "Server;" && line != "EndServer;" && this->Error != 1)
				;
			if (line == "EndServer;")
				this->NbServer++;
			else
			{
				std::cerr << "\e[41mError srever not closed\e[49m" << std::endl;
				this->Error = 1;
				this->NbServer = -1;
				break;
			}
		}
	}
	infile.close();
};

void Config::ParseServer(std::string data)
{
	bool startRoute = false;
	ServerConfig config;
	Route currentRoute;

	std::istringstream iss(data);
	std::string line;

	while (std::getline(iss, line))
	{
		std::istringstream issLine(line);
		std::string key, value;

		if (std::getline(issLine, key, ':') && std::getline(issLine, value, ';'))
		{
			key = trim(key, " \t");
			value = trim(value, " \t");

			if (startRoute)
			{
				if (key == "RoutePath" && !value.empty())
					currentRoute.RoutePath = value;
				else if (key == "Root" && !value.empty())
					currentRoute.Root = value;
				else if (key == "Autoindex" && !value.empty())
					currentRoute.Autoindex = value;
				else if (key == "Index" && !value.empty())
					currentRoute.Index = value;
				else if (key == "Cgi_Exec" && !value.empty())
					currentRoute.CgiExec = value;
				else if (key == "Upload_Path" && !value.empty())
					currentRoute.UploadPath = value;
				else if (key == "Redirection" && !value.empty())
					currentRoute.RedirectionURL = value;
				else if (key == "Accepted_Methods" && !value.empty())
				{
					std::stringstream ss(value);
					std::string token;
					while (std::getline(ss, token, ','))
					{
						size_t first = token.find_first_not_of(' ');
						size_t last = token.find_last_not_of(' ');
						if (first != std::string::npos && last != std::string::npos)
							currentRoute.AcceptedMethods.push_back(token.substr(first, last - first + 1));
					}
				}
			}
			else
			{
				if (key == "Port" && !value.empty())
					config.Port = std::stoi(value);
				else if (key == "Host" && !value.empty())
					config.Host = value;
				else if (key == "Server_Names" && !value.empty())
					config.ServerNames = value;
				else if (key == "Error_Page" && !value.empty())
					config.ErrorPage = value;
				else if (key == "Limit_Client_Body_Size" && !value.empty())
					config.LimitClientBodySize = value;
				else if (key == "GlobalRoot" && !value.empty())
					config.GlobalRoot = value;
			}
		}
		else if (line == "Route;")
		{
			startRoute = true;
			currentRoute = Route();
		}
		else if (line == "EndRoute;")
		{
			config.Routes.push_back(currentRoute);
			startRoute = false;
		}
	}
	std::cout << "+--------------------------------------------------------------------+\n";
	std::cout << "| GLOBAL TABLE                                                       |" << std::endl;
	std::cout << "+--------------------------------------------------------------------+\n";
	std::cout << "| Port:                |" << std::setw(44) << "-" + std::to_string(config.Port) << "-|" << std::endl;
	std::cout << "| Host:                |" << std::setw(44) << "-" + config.Host << "-|" << std::endl;
	std::cout << "| ServerNames:         |" << std::setw(44) << "-" + config.ServerNames << "-|" << std::endl;
	std::cout << "| ErrorPage:           |" << std::setw(44) << "-" + config.ErrorPage << "-|" << std::endl;
	std::cout << "| LimitClientBodySize: |" << std::setw(44) << "-" + config.LimitClientBodySize << "-|" << std::endl;
	std::cout << "| GlobalRoot:          |" << std::setw(44) << "-" + config.GlobalRoot << "-|" << std::endl;
	std::cout << "+--------------------------------------------------------------------+\n";
	std::vector<Route>::iterator it;
	for (it = config.Routes.begin(); it != config.Routes.end(); ++it)
	{
		std::cout << "+--------------------------------------------------------------------+\n";
		std::cout << "| RoutePath:           |" << std::setw(44) << "-" + it->RoutePath << "-|" << std::endl;
		std::cout << "| Redirection:         |" << std::setw(44) << "-" + it->RedirectionURL << "-|" << std::endl;
		std::cout << "| Root:                |" << std::setw(44) << "-" + it->Root << "-|" << std::endl;
		std::cout << "| Autoindex:           |" << std::setw(44) << "-" + it->Autoindex << "-|" << std::endl;
		std::cout << "| Index:               |" << std::setw(44) << "-" + it->Index << "-|" << std::endl;
		std::cout << "| Cgi_Exec:            |" << std::setw(44) << "-" + it->CgiExec << "-|" << std::endl;
		std::cout << "| Upload_Path:         |" << std::setw(44) << "-" + it->UploadPath << "-|" << std::endl;
		std::vector<std::string>::iterator it2;
		for (it2 = it->AcceptedMethods.begin(); it2 != it->AcceptedMethods.end(); ++it2)
		std::cout << "| Accepted_Methods     |" << std::setw(44) << "-" + *it2 << "-|" << std::endl;
		std::cout << "+--------------------------------------------------------------------+\n";
	}
}

Config::Config(std::string filename) : Error(0), NbServer(0)
{
	std::ifstream infile;
	infile.open(filename.c_str());
	if (!infile.is_open())
	{
		std::cerr << "Error openning file " << filename << std::endl;
		this->Error = 1;
	}
	countNbServer(filename);
	std::cout << this->NbServer << std::endl;
	if (this->Error != 1)
	{
		for (int i = 0; i < this->NbServer; i++)
		{
			std::string data;
			data = SpiltServer(infile);
			ParseServer(data);
		}
	}
	infile.close();
}

Config::~Config(void)
{
}
