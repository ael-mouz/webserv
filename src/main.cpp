#include "../include/Config/Config.hpp"
#include "../include/Config/ServerConf.hpp"
#include "../include/Server/Utils.hpp"
#include "../include/Server/Client.hpp"
#include "../include/Server/RunServers.hpp"

int main(int ac, char **av) {
    try {
        if (ac != 2)
            throw std::invalid_argument("Usage: ./webserv [configuration file]");
        RunServers servers(av);

        servers.runing();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
