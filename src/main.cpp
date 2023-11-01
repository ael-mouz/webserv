#include "../include/Config/Config.hpp"
#include "../include/Config/ServerConf.hpp"
#include "../include/Server/Utils.hpp"
// #include "../include/Server/Server.hpp"
#include "../include/Server/Client.hpp"
#include "../include/Server/RunServers.hpp"

int main(int ac, char **av)
{
	try {
		if (ac != 2)
			throw std::invalid_argument("Usage: ./webserv [configuration file]");
		Config config;
		config.parser(av[1]);
		config.checkServerConfig(av[1]);
		config.filterServerConfig();
#ifdef DEBUG_C
		config.printServers();
#endif
        RunServers servers(config.getServerConfig());

        servers.runing();
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
