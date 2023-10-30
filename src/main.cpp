/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yettabaa <yettabaa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/13 19:47:52 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/30 16:15:07 by yettabaa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Include/Config/Config.hpp"
#include "../Include/Utils.hpp"
#include <stdexcept>

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
    // config.printConfig();
    config.printServers();
    // std::cout << config.getNbServer() << std::endl;
#endif
        for (int i = 0; i < config.getNbServer(); i++)
            StartSrever(config.getServerConfig(i).DefaultServerConfig);
    // start_serving();
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
    }
    return 0;
}