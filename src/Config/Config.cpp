#include "../../include/Config/Config.hpp"
#include "../../include/Server/Utils.hpp"

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
void ConfigErrors(const std::string &filename, int i, const std::string &line, const std::string &errormsg)
{
	std::cout << BOLD << filename << ":" << i << ":0: " << FG_RED << "error: " << RESET_ALL << BOLD << errormsg << RESET_ALL << "\n\t" << line << std::endl, exit(1);
}

void Config::parser(std::string filename)
{
	std::ifstream infile(filename.c_str());
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
			ConfigErrors(filename, i, line, "Invalide line");
		if (!line.empty() && line[0] != '#' && line[line.length() - 1] != ';')
			ConfigErrors(filename, i, line, "Expected ';' at end of declaration");
		else if ((line == "Server;" && ServerScop == true) || (line == "EndServer;" && ServerScop == false))
			ConfigErrors(filename, i, line, "Server not closed");
		else if (line == "Server;" && ServerScop == false)
		{
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
			ConfigErrors(filename, i, line, "Invalid line");
	}
	if (infile.eof() && ServerScop == true)
		ConfigErrors(filename, i, line, "Server not closed");
	infile.close();
	checkServerConfig(filename);
	filterServerConfig();
}

void Config::parseServer(const std::string &data, ServerConfig &serverConfig, int start, const std::string &filename)
{
	Route route;
	bool RouteScop = false;
	std::istringstream iss(data);
	std::string routedata, line, key, value;
	int start2 = 0, p = 0, h = 0, s = 0, e = 0, l = 0, t = 0;
	while (std::getline(iss, line))
	{
		start++;
		std::istringstream issline(line);
		if ((line.empty() || line[0] == '#') && RouteScop == false)
			continue;
		if ((line == "Route;" && RouteScop == true) || (line == "EndRoute;" && RouteScop == false))
			ConfigErrors(filename, start, line, "Route not closed");
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
				parsePath(serverConfig.ErrorPage, start, line, filename, 1, 0);
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
			else if (key == "CgiTimeout")
			{
				!value.empty() ? serverConfig.CgiTimeout = value : serverConfig.CgiTimeout = "", t++;
				parseCgiTimeout(serverConfig.CgiTimeout, start, line, filename);
			}
			else
				ConfigErrors(filename, start, line, "Invalid line");
			if (p > 1 || h > 1 || s > 1 || e > 1 || l > 1 || t > 1)
				ConfigErrors(filename, start, line, "Duplicate line");
		}
		else if (RouteScop == true)
			routedata += line + "\n";
		else
			ConfigErrors(filename, start, line, "Invalid line");
	}
	if (iss.eof() && RouteScop == true)
		ConfigErrors(filename, start, line, "Route not closed");
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
				ConfigErrors(filename, start, line, "Route need a RoutePath");
			else if (key == "RoutePath")
			{
				!value.empty() ? route.RoutePath = value : route.RoutePath = "", p++;
				parseRoutePath(route.RoutePath, start, line, filename);
			}
			else if (key == "Root")
			{
				!value.empty() ? route.Root = value : route.Root = "", ro++;
				parsePath(route.Root, start, line, filename, 1, 0);
			}
			else if (key == "Index")
			{
				!value.empty() ? route.Index = value : route.Index = "", i++;
				parsePath(route.Index, start, line, filename, 0, 0);
			}
			else if (key == "Cgi_Exec")
			{
				!value.empty() ? route.CgiExec = value : route.CgiExec = "", c++;
				parsePath(route.CgiExec, start, line, filename, 1, 0);
			}
			else if (key == "Autoindex")
			{
				!value.empty() ? route.Autoindex = value : route.Autoindex = "", a++;
				parseAutoindex(route.Autoindex, start, line, filename);
			}
			else if (key == "Upload_Path")
			{
				!value.empty() ? route.UploadPath = value : route.UploadPath = "", u++;
				parsePath(route.UploadPath, start, line, filename, 1, 1);
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
				ConfigErrors(filename, start, line, "Invalid line");
			if (p > 1 || ro > 1 || i > 1 || c > 1 || a > 1 || u > 1 || r > 1 || m > 1)
				ConfigErrors(filename, start, line, "Duplicate line");
		}
	}
	if (ro + i + c + a + u + r + m < 1)
		ConfigErrors(filename, start, line, "Invalid route, must have at least one option");
}

void Config::parsePort(std::string &port, int start, const std::string &line, const std::string &filename)
{
	if (port.empty())
		ConfigErrors(filename, start, line, "Empty port");
	if (!isDigit(port))
		ConfigErrors(filename, start, line, "Invalid port");
	std::stringstream iss;
	iss << port;
	unsigned int portt;
	iss >> portt;
	if (iss.fail() || (portt < 0 || portt > 65535))
		ConfigErrors(filename, start, line, "Invalid port");
}

void Config::parseServerNameAndHostName(std::string &serverName, int start, const std::string &line, const std::string &filename, int h)
{
	std::string validServerNameCharacters = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-.:";
	if (serverName.empty())
		ConfigErrors(filename, start, line, "Empty Server or host name");
	if (serverName.length() > 253)
		ConfigErrors(filename, start, line, "Invalid Server or host name");
	if (serverName.find_first_not_of(validServerNameCharacters) != std::string::npos)
		ConfigErrors(filename, start, line, "Invalid Server or host name");
	if (h)
		getIpv4Address(serverName, start, line, filename);
}

void Config::parsePath(std::string &path, int start, const std::string &line, const std::string &filename, int i, int j)
{
	std::string validCharacterPath = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 `~!@#$%^&*()_+=-{}[]:;\"\'?/><.,";
	if (path.empty())
		ConfigErrors(filename, start, line, "Empty path");
	if (path.find_first_not_of(validCharacterPath) != std::string::npos)
		ConfigErrors(filename, start, line, "Invalid character path");
	path = getRealPath(path);
	if (i)
		if (access(path.c_str(), F_OK) != 0)
			ConfigErrors(filename, start, line, "Invalid path");
	if (j)
		if (path[path.length() - 1] != '/')
			path += "/";
}

void Config::parseRoutePath(std::string &path, int start, const std::string &line, const std::string &filename)
{
	std::string validCharacterPath = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%";
	if (path.empty())
		ConfigErrors(filename, start, line, "Empty route path");
	if (path.find_first_not_of(validCharacterPath) != std::string::npos)
		ConfigErrors(filename, start, line, "Invalid character route path");
	if (path[path.length() - 1] == '/' && path.length() != 1)
		path.erase(path.end() - 1);
}

void Config::parseAutoindex(const std::string &value, int start, const std::string &line, const std::string &filename)
{
	if (value.empty())
		ConfigErrors(filename, start, line, "Empty Autoindex");
	if (value != "on")
		ConfigErrors(filename, start, line, "Invalid Autoindex");
}
void Config::parseCgiTimeout(std::string &value, int start, const std::string &line, const std::string &filename)
{
	if (value.empty())
		ConfigErrors(filename, start, line, "Empty CgiTimeout");
	if (!isDigit(value))
		ConfigErrors(filename, start, line, "Timeout must be Digits");
	std::stringstream str;
	str << value;
	unsigned int time;
	str >> time;
	if (str.fail())
		ConfigErrors(filename, start, line, "Invalid Timeout");
	value = intToString(time);
};
void Config::parseMethods(Route &route, int start, const std::string &line, const std::string &filename)
{
	if (route.Accepted_Methods.empty())
		ConfigErrors(filename, start, line, "Empty Method");
	if (route.Accepted_Methods[route.Accepted_Methods.length() - 1] == ',')
		ConfigErrors(filename, start, line, "Invalid Method");
	std::istringstream methods(route.Accepted_Methods);
	std::string method;
	int i = 0;
	route.Accepted_Methods_ = "defualt";
	route.Accepted_Methods__ = "defualt";
	route.Accepted_Methods___ = "defualt";
	while (std::getline(methods, method, ','))
	{
		std::string methodss = trim(method, " \t");
		if (methodss.empty())
			ConfigErrors(filename, start, line, "Invalid Method");
		if (methodss != "GET" && methodss != "POST" && methodss != "DELETE")
			ConfigErrors(filename, start, line, "Invalid Method");
		if (i == 0)
			route.Accepted_Methods_ = methodss;
		else if (i == 1)
			route.Accepted_Methods__ = methodss;
		else if (i == 2)
			route.Accepted_Methods___ = methodss;
		else if (i > 2)
			ConfigErrors(filename, start, line, "Invalid Method");
		i++;
	}
	route.Accepted_Methods = "on";
}

void Config::parseRedirection(Route &route, int start, const std::string &line, const std::string &filename)
{
	if (route.Redirection.empty())
		ConfigErrors(filename, start, line, "Empty redirection");
	if (route.Redirection[route.Redirection.length() - 1] == ',')
		ConfigErrors(filename, start, line, "Invalid redirection");
	std::istringstream redirect(route.Redirection);
	std::string redirects;
	int i = 0;
	while (std::getline(redirect, redirects, ','))
	{
		std::string redirectss = trim(redirects, " \t");
		if (redirectss.empty())
			ConfigErrors(filename, start, line, "Invalid redirection");
		if (i == 0)
		{
			if (!isDigit(redirectss))
				ConfigErrors(filename, start, line, "Invalid redirection");
			int status = std::atoi(redirectss.c_str());
			if (status < 300 || status > 307 || status == 306)
				ConfigErrors(filename, start, line, "Invalid redirection");
			route.RedirectionStatus = redirectss;
		}
		else if (i == 1)
			route.RedirectionURL = redirectss;
		else if (i > 1)
			ConfigErrors(filename, start, line, "Invalid redirection");
		i++;
	}
	route.Redirection = "on";
}

void Config::parseBodySize(std::string &sizeStr, int start, const std::string &line, const std::string &filename)
{
	if (sizeStr.empty())
		ConfigErrors(filename, start, line, "Empty size");
	char unit = '\0';
	if (!isDigit(sizeStr))
	{
		unit = sizeStr[sizeStr.length() - 1];
		unit = std::tolower(unit);
		sizeStr.erase(sizeStr.end() - 1);
		if (!isDigit(sizeStr))
			ConfigErrors(filename, start, line, "Invalid characters in size string");
	}
	else
		unit = 'b';
	size_t size = 0;
	std::istringstream iss(sizeStr);
	iss >> size;
	if (iss.fail())
		ConfigErrors(filename, start, line, "Invalid size value");
	switch (unit)
	{
	case 'b':
		if (size > 1073741824000)
			ConfigErrors(filename, start, line, "Size too long > 1073741824000 B");
		size *= 1;
		break;
	case 'k':
		if (size > 1048576000)
			ConfigErrors(filename, start, line, "Size too long > 1048576000 K");
		size *= 1024;
		break;
	case 'm':
		if (size > 1024000)
			ConfigErrors(filename, start, line, "Size too long > 1024000 M");
		size *= 1048576;
		break;
	case 'g':
		if (size > 1000)
			ConfigErrors(filename, start, line, "Size too long > 1000 G");
		size *= 1073741824;
		break;
	default:
		ConfigErrors(filename, start, line, "Unsupported unit");
	}
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
		ConfigErrors(filename, start, line, std::string(gai_strerror(status)));
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
	std::string key, line;
	for (it = this->Serverconfig.begin(); it != this->Serverconfig.end(); ++it)
	{
		key = it->Host + ":" + it->Port + "|" + it->ServerNames;
		res = tempmap.insert(std::make_pair(key, 1));
		line = it->Host + ":" + it->Port + "    ↦   " + it->ServerNames;
		if (!res.second)
			ConfigErrors(filename, 0, line, "Duplicated server");
	}
	ServerConfig serverConfig;
	if (this->Serverconfig.size() == 0)
		Serverconfig.push_back(serverConfig);
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
	int i = 0;
	for (it = this->Serverconfig.begin(); it != this->Serverconfig.end(); it++)
	{
		ft_print_config(++i, *it, 1);
		std::vector<Route>::iterator it1;
		int h = 0;
		for (it1 = it->Routes.begin(); it1 != it->Routes.end(); it1++)
			ft_print_routes(++h, *it1, it->ServerNames);
	}
}

void Config::printServers(void)
{
	std::vector<ServerConf>::iterator it;
	int h = 0;
	for (it = this->Servers_.begin(); it != this->Servers_.end(); ++it)
	{
		ft_print_config(++h, it->DefaultServerConfig, 1);
		std::vector<Route>::iterator it2;
		int b = 0;
		for (it2 = it->DefaultServerConfig.Routes.begin(); it2 != it->DefaultServerConfig.Routes.end(); it2++)
			ft_print_routes(++b, *it2, it->DefaultServerConfig.ServerNames);
		std::map<std::string, ServerConfig>::iterator it1;
		int f = 0;
		for (it1 = it->OtherServerConfig.begin(); it1 != it->OtherServerConfig.end(); ++it1)
		{
			ft_print_config(++f, it1->second, 0);
			std::vector<Route>::iterator it3;
			int c = 0;
			for (it3 = it->DefaultServerConfig.Routes.begin(); it3 != it->DefaultServerConfig.Routes.end(); it3++)
				ft_print_routes(++c, *it3, it1->second.ServerNames);
		}
	}
}
