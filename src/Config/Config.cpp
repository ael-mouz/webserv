/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mouz <ael-mouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/13 19:59:44 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/30 20:43:22 by ael-mouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Config/Config.hpp"

// === Constructors ===

Config::Config(void) : NbServer(0) {}

Config::~Config(void) {}

// === Getter Methods ===

int Config::getNbServer(void) const { return (this->NbServer); }

std::vector<ServerConf> &Config::getServerConfig()
{
	return this->Servers_;
}

// === Member Functions ===

void Config::parser(std::string filename)
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
		if ((line.empty() || line[0] == '#') && ServerScop == false)
			continue;
		int count = 0;
		for (size_t j = 0; j < line.length(); j++)
			if (line[j] == ';')
				count++;
		if (count > 1)
			std::cout << BOLD + filename + ":" << i << ":0: " FG_RED "error:" RESET_ALL "" BOLD "invalide line" RESET_ALL "\n\t" << line << std::endl, exit(1);
		if (!line.empty() && line[0] != '#' && line[line.length() - 1] != ';')
			std::cout << BOLD + filename + ":" << i << ":0: " FG_RED "error:" RESET_ALL "" BOLD "expected ';' at end of declaration" RESET_ALL "\n\t" << line << std::endl, exit(1);
		else if ((line == "Server;" && ServerScop == true) || (line == "EndServer;" && ServerScop == false))
			std::cout << BOLD + filename + ":" << i << ":0: " FG_RED "error:" RESET_ALL "" BOLD " server not closed" RESET_ALL "\n\t" << line << std::endl, exit(1);
		else if (line == "Server;" && ServerScop == false)
		{
			// NbServer++;
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
	if (infile.eof() && ServerScop == true)
		std::cout << BOLD + filename + ":" << i << ":0: " FG_RED "error:" RESET_ALL "" BOLD " server not closed" RESET_ALL "\n\t" << line << std::endl, exit(1);
	infile.close();
}

void Config::parseServer(const std::string &data, ServerConfig &serverConfig, int start, const std::string &filename)
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
		if ((line.empty() || line[0] == '#') && RouteScop == false)
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
			if (key == "Host")
			{
				!value.empty() ? serverConfig.Host = value : serverConfig.Host = "", h++;
				parseServerNameAndHostName(serverConfig.Host, start, line, filename, 1);
			}
			else if (key == "Server_Names")
			{
				!value.empty() ? serverConfig.ServerNames = value : serverConfig.ServerNames = "", s++;
				parseServerNameAndHostName(serverConfig.ServerNames, start, line, filename, 0);
			}
			else if (key == "Error_Page")
			{
				!value.empty() ? serverConfig.ErrorPage = value : serverConfig.ErrorPage = "", e++;
				parsePath(serverConfig.ErrorPage, start, line, filename);
			}
			else if (key == "Port")
			{
				!value.empty() ? serverConfig.Port = value : serverConfig.Port = "", p++;
				parsePort(serverConfig.Port, start, line, filename);
			}
			else if (key == "Limit_Client_Body_Size")
			{
				!value.empty() ? serverConfig.LimitClientBodySize = value : serverConfig.LimitClientBodySize = "", l++;
				parseBodySize(serverConfig.LimitClientBodySize, start, line, filename);
			}
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
	if (iss.eof() && RouteScop == true)
		std::cout << BOLD + filename + ":" << start << ":0: " FG_RED "error:" RESET_ALL "" BOLD " route not closed" RESET_ALL "\n\t" << line << std::endl, exit(1);
}

void Config::parseRoute(const std::string &data, Route &route, int start, const std::string &filename)
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
			{
				!value.empty() ? route.RoutePath = value : route.RoutePath = "", p++;
				parseRoutePath(route.RoutePath, start, line, filename);
			}
			else if (key == "Root")
			{
				!value.empty() ? route.Root = value : route.Root = "", ro++;
				parsePath(route.Root, start, line, filename);
			}
			else if (key == "Index")
			{
				!value.empty() ? route.Index = value : route.Index = "", i++;
				parsePath(route.Index, start, line, filename);
			}
			else if (key == "Cgi_Exec")
			{
				!value.empty() ? route.CgiExec = value : route.CgiExec = "", c++;
				parsePath(route.CgiExec, start, line, filename);
			}
			else if (key == "Autoindex")
			{
				!value.empty() ? route.Autoindex = value : route.Autoindex = "", a++;
				parseAutoindex(route.Autoindex, start, line, filename);
			}
			else if (key == "Upload_Path")
			{
				!value.empty() ? route.UploadPath = value : route.UploadPath = "", u++;
				parsePath(route.UploadPath, start, line, filename);
			}
			else if (key == "Redirection")
			{
				!value.empty() ? route.Redirection = value : route.Redirection = "", r++;
				parseRedirection(route, start, line, filename);
			}
			else if (key == "Accepted_Methods")
			{
				!value.empty() ? route.Accepted_Methods = value : route.Accepted_Methods = "", m++;
				parseMethods(route, start, line, filename);
			}
			else
				std::cout << BOLD + filename + ":" << start << ":0: " FG_RED "error:" RESET_ALL "" BOLD " invalid line" RESET_ALL "\n\t" << line << std::endl, exit(1);
			if (p > 1 || ro > 1 || i > 1 || c > 1 || a > 1 || u > 1 || r > 1 || m > 1)
				std::cout << BOLD + filename + ":" << start << ":0: " FG_RED "error:" RESET_ALL "" BOLD " duplicate line" RESET_ALL "\n\t" << line << std::endl, exit(1);
		}
	}
	if (ro + i + c + a + u + r + m < 1)
		std::cout << BOLD + filename + ":" << start << ":0: " FG_RED "error:" RESET_ALL "" BOLD " invalide route must have at least one option" RESET_ALL "\n\t" << line << std::endl, exit(1);
}

void Config::parsePort(std::string &port, int start, const std::string &line, const std::string &filename)
{
	if (port.empty())
		std::cout << BOLD + filename + ":" << start << ":0: " FG_RED "error:" RESET_ALL "" BOLD " empty port" RESET_ALL "\n\t" << line << std::endl, exit(1);
	if (!isDigit(port))
		std::cout << BOLD + filename + ":" << start << ":0: " FG_RED "error:" RESET_ALL "" BOLD " Invalid port" RESET_ALL "\n\t" << line << std::endl, exit(1);
	int portt = std::atoi(port.c_str());
	if (portt < 0 || portt > 65535)
		std::cout << BOLD + filename + ":" << start << ":0: " FG_RED "error:" RESET_ALL "" BOLD " Invalid port" RESET_ALL "\n\t" << line << std::endl, exit(1);
}

void Config::parseServerNameAndHostName(std::string &serverName, int start, const std::string &line, const std::string &filename, int h)
{
	std::string validServerNameCharacters = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-.";
	if (serverName.empty())
		std::cout << BOLD + filename + ":" << start << ":0: " FG_RED "error:" RESET_ALL "" BOLD " empty Server or host name" RESET_ALL "\n\t" << line << std::endl, exit(1);
	if (serverName.length() > 253)
		std::cout << BOLD + filename + ":" << start << ":0: " FG_RED "error:" RESET_ALL "" BOLD " Invalid Server or host name" RESET_ALL "\n\t" << line << std::endl, exit(1);
	if (serverName.find_first_not_of(validServerNameCharacters) != std::string::npos)
		std::cout << BOLD + filename + ":" << start << ":0: " FG_RED "error:" RESET_ALL "" BOLD " Invalid Server or host name" RESET_ALL "\n\t" << line << std::endl, exit(1);
	if (h)
		getIpv4Address(serverName, start, line, filename);
}

void Config::parsePath(const std::string &path, int start, const std::string &line, const std::string &filename)
{
	std::string validCharacterPath = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 `~!@#$%^&*()_+=-{}[]:;\"\'?/><.,";
	if (path.empty())
		std::cout << BOLD + filename + ":" << start << ":0: " FG_RED "error:" RESET_ALL "" BOLD " empty path" RESET_ALL "\n\t" << line << std::endl, exit(1);
	if (path.find_first_not_of(validCharacterPath) != std::string::npos)
		std::cout << BOLD + filename + ":" << start << ":0: " FG_RED "error:" RESET_ALL "" BOLD " Invalid character path" RESET_ALL "\n\t" << line << std::endl, exit(1);
}

void Config::parseRoutePath(const std::string &path, int start, const std::string &line, const std::string &filename)
{
	std::string validCharacterPath = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%";
	if (path.empty())
		std::cout << BOLD + filename + ":" << start << ":0: " FG_RED "error:" RESET_ALL "" BOLD " empty route path" RESET_ALL "\n\t" << line << std::endl, exit(1);
	if (path.find_first_not_of(validCharacterPath) != std::string::npos)
		std::cout << BOLD + filename + ":" << start << ":0: " FG_RED "error:" RESET_ALL "" BOLD " Invalid character route path" RESET_ALL "\n\t" << line << std::endl, exit(1);
}

void Config::parseAutoindex(const std::string &value, int start, const std::string &line, const std::string &filename)
{
	if (value.empty())
		std::cout << BOLD + filename + ":" << start << ":0: " FG_RED "error:" RESET_ALL "" BOLD " empty Autoindex" RESET_ALL "\n\t" << line << std::endl, exit(1);
	if (value != "on")
		std::cout << BOLD + filename + ":" << start << ":0: " FG_RED "error:" RESET_ALL "" BOLD " Invalid Autoindex" RESET_ALL "\n\t" << line << std::endl, exit(1);
}

void Config::parseMethods(Route &route, int start, const std::string &line, const std::string &filename)
{
	if (route.Accepted_Methods.empty())
		std::cout << BOLD + filename + ":" << start << ":0: " FG_RED "error:" RESET_ALL "" BOLD " empty Method" RESET_ALL "\n\t" << line << std::endl, exit(1);
	if (route.Accepted_Methods[route.Accepted_Methods.length() - 1] == ',')
		std::cout << BOLD + filename + ":" << start << ":0: " FG_RED "error:" RESET_ALL "" BOLD " invalid Method" RESET_ALL "\n\t" << line << std::endl, exit(1);
	std::istringstream methods(route.Accepted_Methods);
	std::string method;
	int i = 0;
	while (std::getline(methods, method, ','))
	{
		std::string methodss = trim(method, " \t");
		if (methodss.empty())
			std::cout << BOLD + filename + ":" << start << ":0: " FG_RED "error:" RESET_ALL "" BOLD " invalid Method" RESET_ALL "\n\t" << line << std::endl, exit(1);
		if (methodss != "GET" && methodss != "POST" && methodss != "DELETE")
			std::cout << BOLD + filename + ":" << start << ":0: " FG_RED "error:" RESET_ALL "" BOLD " invalid Method" RESET_ALL "\n\t" << line << std::endl, exit(1);
		if (i == 0)
			route.Accepted_Methods_ = methodss;
		else if (i == 1)
			route.Accepted_Methods__ = methodss;
		else if (i == 2)
			route.Accepted_Methods___ = methodss;
		else if (i > 2)
			std::cout << BOLD + filename + ":" << start << ":0: " FG_RED "error:" RESET_ALL "" BOLD " invalid Methods" RESET_ALL "\n\t" << line << std::endl, exit(1);
		i++;
	}
	route.Accepted_Methods = "on";
}

void Config::parseRedirection(Route &route, int start, const std::string &line, const std::string &filename)
{
	if (route.Redirection.empty())
		std::cout << BOLD + filename + ":" << start << ":0: " FG_RED "error:" RESET_ALL "" BOLD " empty redirection" RESET_ALL "\n\t" << line << std::endl, exit(1);
	if (route.Redirection[route.Redirection.length() - 1] == ',')
		std::cout << BOLD + filename + ":" << start << ":0: " FG_RED "error:" RESET_ALL "" BOLD " invalid redirection" RESET_ALL "\n\t" << line << std::endl, exit(1);
	std::istringstream redirect(route.Redirection);
	std::string redirects;
	int i = 0;
	while (std::getline(redirect, redirects, ','))
	{
		std::string redirectss = trim(redirects, " \t");
		if (redirectss.empty())
			std::cout << BOLD + filename + ":" << start << ":0: " FG_RED "error:" RESET_ALL "" BOLD " invalid redirection" RESET_ALL "\n\t" << line << std::endl, exit(1);
		if (i == 0)
		{
			if (!isDigit(redirectss))
				std::cout << BOLD + filename + ":" << start << ":0: " FG_RED "error:" RESET_ALL "" BOLD " invalid redirection status" RESET_ALL "\n\t" << line << std::endl, exit(1);
			int status = std::atoi(redirectss.c_str());
			if (status < 300 || status > 307 || status == 306)
				std::cout << BOLD + filename + ":" << start << ":0: " FG_RED "error:" RESET_ALL "" BOLD " invalid redirection status" RESET_ALL "\n\t" << line << std::endl, exit(1);
			route.RedirectionStatus = redirectss;
		}
		else if (i == 1)
			route.RedirectionURL = redirectss;
		else if (i > 1)
			std::cout << BOLD + filename + ":" << start << ":0: " FG_RED "error:" RESET_ALL "" BOLD " invalid redirection" RESET_ALL "\n\t" << line << std::endl, exit(1);
		i++;
	}
	route.Redirection = "on";
}

void Config::parseBodySize(std::string &sizeStr, int start, const std::string &line, const std::string &filename)
{
	if (sizeStr.empty())
		std::cout << BOLD + filename + ":" << start << ":0: " FG_RED "error:" RESET_ALL "" BOLD " empty size" RESET_ALL "\n\t" << line << std::endl, exit(1);
	std::istringstream iss(sizeStr);
	size_t size = 0;
	char unit = '\0';
	if (!(iss >> size))
		std::cout << BOLD + filename + ":" << start << ":0: " FG_RED "error:" RESET_ALL "" BOLD " Invalid size value" RESET_ALL "\n\t" << line << std::endl, exit(1);
	if (iss >> unit)
	{
		unit = std::tolower(unit);
		switch (unit)
		{
		case 'k':
			size *= 1024;
			break;
		case 'm':
			size *= 1024 * 1024;
			break;
		case 'g':
			size *= 1024 * 1024 * 1024;
			break;
		default:
			std::cout << BOLD + filename + ":" << start << ":0: " FG_RED "error:" RESET_ALL "" BOLD " Unsupported unit" RESET_ALL "\n\t" << line << std::endl, exit(1);
		}
	}
	std::string remaining;
	if (iss >> remaining)
		std::cout << BOLD + filename + ":" << start << ":0: " FG_RED "error:" RESET_ALL "" BOLD " Invalid characters in size string" RESET_ALL "\n\t" << line << std::endl, exit(1);
	sizeStr = intToString(size);
}

void Config::getIpv4Address(std::string &hostname, int start, const std::string &line, const std::string &filename)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	std::string ipAddress;

	std::memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	int status = getaddrinfo(hostname.c_str(), NULL, &hints, &result);
	if (status != 0)
		std::cout << BOLD + filename + ":" << start << ":0: " FG_RED "error:" RESET_ALL " " BOLD << std::string(gai_strerror(status)) << RESET_ALL "\n\t" << line << std::endl, exit(1);
	for (rp = result; rp != NULL; rp = rp->ai_next)
	{
		if (rp->ai_family == AF_INET)
		{
			void *addr;
			char ipstr[INET_ADDRSTRLEN];
			struct sockaddr_in *ipv4 = (struct sockaddr_in *)rp->ai_addr;
			addr = &(ipv4->sin_addr);
			if (inet_ntop(rp->ai_family, addr, ipstr, sizeof(ipstr)) != NULL)
			{
				ipAddress = ipstr;
				break;
			}
		}
	}
	freeaddrinfo(result);
	hostname = ipAddress;
}

void Config::checkServerConfig(const std::string &filename)
{
	std::map<std::string, int> tempmap;
	std::pair<std::map<std::string, int>::iterator, bool> res;
	std::vector<ServerConfig>::iterator it;
	std::string key;
	for (it = this->Serverconfig.begin(); it != this->Serverconfig.end(); ++it)
	{
		key = it->Host + ":" + it->Port + "|" + it->ServerNames;
		res = tempmap.insert(std::make_pair(key, 1));
		if (!res.second)
			std::cout << BOLD + filename + ":0:0: " FG_RED "error:" RESET_ALL " " BOLD "Duplicated server " RESET_ALL "\n\t " << it->Host + ":" + it->Port + "    ↦   " + it->ServerNames << std::endl, exit(1);
	}
}

void Config::filterServerConfig()
{
	while (!this->Serverconfig.empty())
	{
		ServerConf server_;
		std::vector<ServerConfig>::iterator it = this->Serverconfig.begin();
		std::string key = it->Host + ":" + it->Port;
		server_.DefaultServerConfig = *it;
		this->Serverconfig.erase(it);
		for (it = this->Serverconfig.begin(); it != this->Serverconfig.end();)
		{
			std::string key1 = it->Host + ":" + it->Port;
			if (key1 == key)
			{
				server_.OtherServerConfig.insert(std::make_pair(it->ServerNames, *it));
				it = this->Serverconfig.erase(it);
			}
			else
				++it;
		}
		this->Servers_.push_back(server_);
	}
	this->NbServer = Servers_.size();
}
void Config::printConfig(void)
{
	std::vector<ServerConfig>::iterator it;
	for (it = this->Serverconfig.begin(); it != this->Serverconfig.end(); ++it)
	{
		std::cout << "╔═══════════════════════════════════════════════════════════════════════════╗" << std::endl;
		std::cout << "║" BOLD " GLOBAL TABLE " RESET_ALL << std::setw(64) << "║" << std::endl;
		std::cout << "╠═════════════════╦═════════════════════════════════════════════════════════╣" << std::endl;
		std::cout << "║ Port            ║" << std::setw(58) << "▻" + it->Port << "◅║" << std::endl;
		std::cout << "║ Host            ║" << std::setw(58) << "▻" + it->Host << "◅║" << std::endl;
		std::cout << "║ ServerNames     ║" << std::setw(58) << "▻" + it->ServerNames << "◅║" << std::endl;
		std::cout << "║ ErrorPage       ║" << std::setw(58) << "▻" + it->ErrorPage << "◅║" << std::endl;
		std::cout << "║ LimitBodySize   ║" << std::setw(58) << "▻" + it->LimitClientBodySize << "◅║" << std::endl;
		std::cout << "║ GlobalRoot      ║" << std::setw(58) << "▻" + it->GlobalRoot << "◅║" << std::endl;
		std::cout << "╚═════════════════╩═════════════════════════════════════════════════════════╝" << std::endl;
		std::vector<Route>::iterator it1;
		int h = 0;
		for (it1 = it->Routes.begin(); it1 != it->Routes.end(); ++it1)
		{
			h++;
			std::cout << "↦  ╔═══════════════════════════════════════════════════════════════════════════╗" << std::endl;
			std::cout << "↦  ║" FG_BLUE BOLD " ROUTE " << h << RESET_ALL << std::setw(70) << "║" << std::endl;
			std::cout << "↦  ╠═════════════════╦═════════════════════════════════════════════════════════╣\n";
			std::cout << "↦  ║ RoutePath       ║" << std::setw(58) << "▻" + it1->RoutePath << "◅║" << std::endl;
			std::cout << "↦  ║ UploadPath      ║" << std::setw(58) << "▻" + it1->UploadPath << "◅║" << std::endl;
			std::cout << "↦  ║ Redirection     ║" << std::setw(58) << "▻" + it1->Redirection << "◅║" << std::endl;
			std::cout << "↦  ║ RedirectStatus  ║" << std::setw(58) << "▻" + it1->RedirectionStatus << "◅║" << std::endl;
			std::cout << "↦  ║ RedirectionURL  ║" << std::setw(58) << "▻" + it1->RedirectionURL << "◅║" << std::endl;
			std::cout << "↦  ║ Root            ║" << std::setw(58) << "▻" + it1->Root << "◅║" << std::endl;
			std::cout << "↦  ║ Autoindex       ║" << std::setw(58) << "▻" + it1->Autoindex << "◅║" << std::endl;
			std::cout << "↦  ║ Index           ║" << std::setw(58) << "▻" + it1->Index << "◅║" << std::endl;
			std::cout << "↦  ║ Cgi_Exec        ║" << std::setw(58) << "▻" + it1->CgiExec << "◅║" << std::endl;
			std::cout << "↦  ║ Methods         ║" << std::setw(58) << "▻" + it1->Accepted_Methods << "◅║" << std::endl;
			std::cout << "↦  ║ Methods1        ║" << std::setw(58) << "▻" + it1->Accepted_Methods_ << "◅║" << std::endl;
			std::cout << "↦  ║ Methods2        ║" << std::setw(58) << "▻" + it1->Accepted_Methods__ << "◅║" << std::endl;
			std::cout << "↦  ║ Methods3        ║" << std::setw(58) << "▻" + it1->Accepted_Methods___ << "◅║" << std::endl;
			std::cout << "↦  ╚═════════════════╩═════════════════════════════════════════════════════════╝" << std::endl;
		}
	}
}

void Config::printServers(void)
{
	std::vector<ServerConf>::iterator it;
	std::cout << BOLD "NUMBER OF SERVERS : " << this->NbServer << std::endl;
	int h = 0;
	for (it = this->Servers_.begin(); it != this->Servers_.end(); ++it)
	{
		h++;
		std::cout << "╔═══════════════════════════════════════════════════════════════════════════╗" << std::endl;
		std::cout << "║" FG_BLUE BOLD " DEFAULT CONFIG " RESET_ALL << h << std::setw(61) << "║" << std::endl;
		std::cout << "╠═════════════════╦═════════════════════════════════════════════════════════╣" << std::endl;
		std::cout << "║ Port            ║" << std::setw(58) << "▻" + it->DefaultServerConfig.Port << "◅║" << std::endl;
		std::cout << "║ Host            ║" << std::setw(58) << "▻" + it->DefaultServerConfig.Host << "◅║" << std::endl;
		std::cout << "║ ServerNames     ║" << std::setw(58) << "▻" + it->DefaultServerConfig.ServerNames << "◅║" << std::endl;
		std::cout << "║ ErrorPage       ║" << std::setw(58) << "▻" + it->DefaultServerConfig.ErrorPage << "◅║" << std::endl;
		std::cout << "║ LimitBodySize   ║" << std::setw(58) << "▻" + it->DefaultServerConfig.LimitClientBodySize << "◅║" << std::endl;
		std::cout << "║ GlobalRoot      ║" << std::setw(58) << "▻" + it->DefaultServerConfig.GlobalRoot << "◅║" << std::endl;
		std::cout << "╚═════════════════╩═════════════════════════════════════════════════════════╝" << std::endl;
		std::map<std::string, ServerConfig>::iterator it1;

		for (it1 = it->OtherServerConfig.begin(); it1 != it->OtherServerConfig.end(); ++it1)
		{
			std::cout << "↦     ▻" BOLD " " << it1->first << " " RESET_ALL "◅" << std::endl;
			std::cout << "↦     ╔═══════════════════════════════════════════════════════════════════════════╗" << std::endl;
			std::cout << "↦     ║" BOLD " OTHER CONFIG " RESET_ALL << std::setw(64) << "║" << std::endl;
			std::cout << "↦     ╠═════════════════╦═════════════════════════════════════════════════════════╣" << std::endl;
			std::cout << "↦     ║ Port            ║" << std::setw(58) << "▻" + it1->second.Port << "◅║" << std::endl;
			std::cout << "↦     ║ Host            ║" << std::setw(58) << "▻" + it1->second.Host << "◅║" << std::endl;
			std::cout << "↦     ║ ServerNames     ║" << std::setw(58) << "▻" + it1->second.ServerNames << "◅║" << std::endl;
			std::cout << "↦     ║ ErrorPage       ║" << std::setw(58) << "▻" + it1->second.ErrorPage << "◅║" << std::endl;
			std::cout << "↦     ║ LimitBodySize   ║" << std::setw(58) << "▻" + it1->second.LimitClientBodySize << "◅║" << std::endl;
			std::cout << "↦     ║ GlobalRoot      ║" << std::setw(58) << "▻" + it1->second.GlobalRoot << "◅║" << std::endl;
			std::cout << "↦     ╚═════════════════╩═════════════════════════════════════════════════════════╝" << std::endl;
		}
	}
}
