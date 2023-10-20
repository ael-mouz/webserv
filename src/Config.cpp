/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mouz <ael-mouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/13 19:59:44 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/20 18:26:06 by ael-mouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Config.hpp"

Config::Config(std::string filename) // TODO multi server same host and same port same server name
{
	std::ifstream infile(filename);
	if (!infile.is_open())
		std::cerr << "Error opening file: " << filename << std::endl, exit(1);
	ServerConfig serverConfig;
	int i = 0, start = 0;
	bool ServerScop = false;
	std::string line;
	std::string data;
	while (std::getline(infile, line))
	{
		line = trim(line, " \t");
		i++;
		if ((line[0] == '#' || line.empty()) && ServerScop == false)
			continue;
		int count = 0;
		for (size_t j = 0; j < line.length(); j++)
			if (line[j] == ';')
				count++;
		if (count > 1)
			std::cout << BOLD + filename + ":" << i << ":0: " FG_RED "error:" RESET_ALL "" BOLD "invalide line" RESET_ALL "\n\t" << line << std::endl, exit(1);
		if (line[line.length() - 1] != ';' && line[0] != '#' && !line.empty())
			std::cout << BOLD + filename + ":" << i << ":0: " FG_RED "error:" RESET_ALL "" BOLD "expected ';' at end of declaration" RESET_ALL "\n\t" << line << std::endl, exit(1);
		else if ((line == "Server;" && ServerScop == true) || (line == "EndServer;" && ServerScop == false))
			std::cout << BOLD + filename + ":" << i << ":0: " FG_RED "error:" RESET_ALL "" BOLD " server not closed" RESET_ALL "\n\t" << line << std::endl, exit(1);
		else if (line == "Server;" && ServerScop == false)
		{
			NbServer++;
			start = i;
			data.clear();
			ServerScop = true;
			serverConfig = ServerConfig();
		}
		else if (line == "EndServer;" && ServerScop == true)
		{
			ServerScop = false;
			parseServer(data, serverConfig, start, filename);
			Serverconfig.push_back(serverConfig);
		}
		else if (ServerScop == true)
			data += line + "\n";
		else
			std::cout << BOLD + filename + ":" << i << ":0: " FG_RED "error:" RESET_ALL "" BOLD " invalid line" RESET_ALL "\n\t" << line << std::endl, exit(1);
	}
	infile.close();
}

void Config::parseServer(const std::string &data, ServerConfig &serverConfig, int start, std::string filename)
{
	Route route;
	bool RouteScop = false;
	std::istringstream iss(data);
	std::string routedata, line, key, value;
	int start2 = 0, p = 0, h = 0, s = 0, e = 0, l = 0;
	while (std::getline(iss, line))
	{
		start++;
		std::istringstream issline(line);
		if ((line[0] == '#' || line.empty()) && RouteScop == false)
			continue;
		if ((line == "Route;" && RouteScop == true) || (line == "EndRoute;" && RouteScop == false))
			std::cout << BOLD + filename + ":" << start << ":0: " FG_RED "error:" RESET_ALL "" BOLD " route not closed" RESET_ALL "\n\t" << line << std::endl, exit(1);
		else if (line == "Route;" && RouteScop == false)
		{
			start2 = start;
			routedata.clear();
			RouteScop = true;
			route = Route();
		}
		else if (line == "EndRoute;" && RouteScop == true)
		{
			RouteScop = false;
			parseRoute(routedata, route, start2, filename);
			serverConfig.Routes.push_back(route);
		}
		else if (RouteScop == false && std::getline(issline, key, ':') && std::getline(issline, value, ';'))
		{
			key = trim(key, " \t");
			value = trim(value, " \t");
			if (key == "Port")
				!value.empty() ? serverConfig.Port = std::atoi(value.c_str()) : serverConfig.Port = 0, p++;
			else if (key == "Host")
				!value.empty() ? serverConfig.Host = value : serverConfig.Host = "", h++;
			else if (key == "Server_Names")
				!value.empty() ? serverConfig.ServerNames = value : serverConfig.ServerNames = "", s++;
			else if (key == "Error_Page")
				!value.empty() ? serverConfig.ErrorPage = value : serverConfig.ErrorPage = "", e++;
			else if (key == "Limit_Client_Body_Size")
				!value.empty() ? serverConfig.LimitClientBodySize = value : serverConfig.LimitClientBodySize = "", l++;
			else
				std::cout << BOLD + filename + ":" << start << ":0: " FG_RED "error:" RESET_ALL "" BOLD " invalid line" RESET_ALL "\n\t" << line << std::endl, exit(1);
			if (p > 1 || h > 1 || s > 1 || e > 1 || l > 1)
				std::cout << BOLD + filename + ":" << start << ":0: " FG_RED "error:" RESET_ALL "" BOLD " duplicate line" RESET_ALL "\n\t" << line << std::endl, exit(1);
		}
		else if (RouteScop == true)
			routedata += line + "\n";
		else
			std::cout << BOLD + filename + ":" << start << ":0: " FG_RED "error:" RESET_ALL "" BOLD " invalid line" RESET_ALL "\n\t" << line << std::endl, exit(1);
	}
}

void Config::parseRoute(const std::string &data, Route &route, int start, std::string filename)
{
	std::istringstream iss(data);
	std::string line, key, value;
	int p = 0, ro = 0, i = 0, c = 0, a = 0, u = 0, r = 0, m = 0;
	while (std::getline(iss, line))
	{
		start++;
		std::istringstream issline(line);
		if (line[0] == '#' || line.empty())
			continue;
		if (std::getline(issline, key, ':') && std::getline(issline, value, ';'))
		{
			key = trim(key, " \t");
			value = trim(value, " \t");
			if (key != "RoutePath" && p == 0)
				std::cout << BOLD + filename + ":" << start << ":0: " FG_RED "error:" RESET_ALL "" BOLD " route need a RoutePath" RESET_ALL "\n\t" << line << std::endl, exit(1);
			else if (key == "RoutePath")
				!value.empty() ? route.RoutePath = value : route.RoutePath = "", p++;
			else if (key == "Root")
				!value.empty() ? route.Root = value : route.Root = "", ro++;
			else if (key == "Index")
				!value.empty() ? route.Index = value : route.Index = "", i++;
			else if (key == "Cgi_Exec")
				!value.empty() ? route.CgiExec = value : route.CgiExec = "", c++;
			else if (key == "Autoindex")
				!value.empty() ? route.Autoindex = value : route.Autoindex = "", a++;
			else if (key == "Upload_Path")
				!value.empty() ? route.UploadPath = value : route.UploadPath = "", u++;
			else if (key == "Redirection")
				!value.empty() ? route.RedirectionURL = value : route.RedirectionURL = "", r++;
			else if (key == "Accepted_Methods")
			{
				if (!value.empty())
				{
					std::istringstream methods(value);
					std::string method;
					while (std::getline(methods, method, ','))
						route.AcceptedMethods.push_back(trim(method, " \t"));
				}
				m++;
			}
			else
				std::cout << BOLD + filename + ":" << start << ":0: " FG_RED "error:" RESET_ALL "" BOLD " invalid line" RESET_ALL "\n\t" << line << std::endl, exit(1);
			if (p > 1 || ro > 1 || i > 1 || c > 1 || a > 1 || u > 1 || r > 1 || m > 1)
				std::cout << BOLD + filename + ":" << start << ":0: " FG_RED "error:" RESET_ALL "" BOLD " duplicate line" RESET_ALL "\n\t" << line << std::endl, exit(1);
		}
	}
}

Config::~Config(void) {}

void Config::printConfig(void)
{
	std::vector<ServerConfig>::iterator it;
	for (it = this->Serverconfig.begin(); it != this->Serverconfig.end(); ++it)
	{
		std::cout << "+--------------------------------------------------------------------+\n";
		std::cout << "| \e[1mGLOBAL TABLE\e[0m                                                       |" << std::endl;
		std::cout << "+--------------------------------------------------------------------+\n";
		std::cout << "| Port:                |" << std::setw(44) << "-" + std::to_string(it->Port) << "-|" << std::endl;
		std::cout << "| Host:                |" << std::setw(44) << "-" + it->Host << "-|" << std::endl;
		std::cout << "| ServerNames:         |" << std::setw(44) << "-" + it->ServerNames << "-|" << std::endl;
		std::cout << "| ErrorPage:           |" << std::setw(44) << "-" + it->ErrorPage << "-|" << std::endl;
		std::cout << "| LimitClientBodySize: |" << std::setw(44) << "-" + it->LimitClientBodySize << "-|" << std::endl;
		std::cout << "| GlobalRoot:          |" << std::setw(44) << "-" + it->GlobalRoot << "-|" << std::endl;
		std::cout << "+--------------------------------------------------------------------+\n";
		std::vector<Route>::iterator it1;
		int h = 0;
		for (it1 = it->Routes.begin(); it1 != it->Routes.end(); ++it1)
		{
			h++;
			std::cout << "+--------------------------------------------------------------------+\n";
			std::cout << "| \e[34m\e[1mROUTE\e[0m " << h << "                                                            |\n";
			std::cout << "+--------------------------------------------------------------------+\n";
			std::cout << "| RoutePath:           |" << std::setw(44) << "-" + it1->RoutePath << "-|" << std::endl;
			std::cout << "| Redirection:         |" << std::setw(44) << "-" + it1->RedirectionURL << "-|" << std::endl;
			std::cout << "| Root:                |" << std::setw(44) << "-" + it1->Root << "-|" << std::endl;
			std::cout << "| Autoindex:           |" << std::setw(44) << "-" + it1->Autoindex << "-|" << std::endl;
			std::cout << "| Index:               |" << std::setw(44) << "-" + it1->Index << "-|" << std::endl;
			std::cout << "| Cgi_Exec:            |" << std::setw(44) << "-" + it1->CgiExec << "-|" << std::endl;
			std::cout << "| Upload_Path:         |" << std::setw(44) << "-" + it1->UploadPath << "-|" << std::endl;
			std::vector<std::string>::iterator it2;
			for (it2 = it1->AcceptedMethods.begin(); it2 != it1->AcceptedMethods.end(); ++it2)
				std::cout << "| Accepted_Methods     |" << std::setw(44) << "-" + *it2 << "-|" << std::endl;
			std::cout << "+--------------------------------------------------------------------+\n";
		}
	}
}