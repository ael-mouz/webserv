#include "../include/Config/Config.hpp"
#include "../include/Config/ServerConf.hpp"
#include "../include/Server/Utils.hpp"
#include "../include/Server/Client.hpp"
#include "../include/Server/RunServers.hpp"

int main(int ac, char **av)
{
	try
	{
		if (ac == 2)
			RunServers(av[1]).runing();
		else if(ac == 1)
			RunServers("./config/Default.conf").runing();
		else
			throw std::invalid_argument("Usage: ./webserv [configuration file]");
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
