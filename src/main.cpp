#include "../include/Config/Config.hpp"
#include "../include/Config/ServerConf.hpp"
#include "../include/Utils.hpp"
#include "../include/server.hpp"

int main(int ac, char **av)
{
	try
	{
		if (ac != 2)
			throw std::invalid_argument("Usage: ./webserv [configuration file]");
		Config config;
		config.parser(av[1]);
		config.checkServerConfig(av[1]);
		config.filterServerConfig();
#ifdef DEBUG_C
		config.printServers();
#endif
		vector<Server> servers;
		vector<ServerConf> ServerConf_ = config.getServerConfig();
		for (vector<ServerConf>::iterator it = ServerConf_.begin(); it != ServerConf_.end(); it++)
		{
			Server serv;
			serv.serverConf = *it;
			serv.init();
			servers.push_back(serv);
		}
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
	}
	return 0;
}
