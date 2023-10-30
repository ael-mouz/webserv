/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mouz <ael-mouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/13 19:47:52 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/30 12:57:05 by ael-mouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Include.hpp"
#include "../include/Server.hpp"
#include "../include/Config.hpp"

int main(int ac, char **av)
{
    if (ac == 2)
    {
        Config config;
        config.parser(av[1]);
        config.checkServerConfig(av[1]);
        config.filterServerConfig();
#ifdef DEBUG_C
        // config.printConfig();
        config.printServers();
        // std::cout << config.getNbServer() << std::endl;
#endif
        std::cout << "╔═══════════════════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║" BOLD " GLOBAL TABLE " RESET_ALL << std::setw(64) << "║" << std::endl;
        std::cout << "╠═════════════════╦═════════════════════════════════════════════════════════╣" << std::endl;
        std::cout << "║ Port            ║" << std::setw(58) << "▻" + config.getServerConfig(0).DefaultServerConfig.Port << "◅║" << std::endl;
        std::cout << "║ Host            ║" << std::setw(58) << "▻" + config.getServerConfig(0).DefaultServerConfig.Host << "◅║" << std::endl;
        std::cout << "║ ServerNames     ║" << std::setw(58) << "▻" + config.getServerConfig(0).DefaultServerConfig.ServerNames << "◅║" << std::endl;
        std::cout << "║ ErrorPage       ║" << std::setw(58) << "▻" + config.getServerConfig(0).DefaultServerConfig.ErrorPage << "◅║" << std::endl;
        std::cout << "║ LimitBodySize   ║" << std::setw(58) << "▻" + config.getServerConfig(0).DefaultServerConfig.LimitClientBodySize << "◅║" << std::endl;
        std::cout << "║ GlobalRoot      ║" << std::setw(58) << "▻" + config.getServerConfig(0).DefaultServerConfig.GlobalRoot << "◅║" << std::endl;
        std::cout << "╚═════════════════╩═════════════════════════════════════════════════════════╝" << std::endl;
        StartSrever(config.getServerConfig(0).DefaultServerConfig);
        return 0;
    }
    return 0;
}
