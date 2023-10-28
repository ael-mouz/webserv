/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mouz <ael-mouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/13 19:47:52 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/28 21:12:52 by ael-mouz         ###   ########.fr       */
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
        StartSrever(config.getServerConfig(0));
        return 0;
    }
    return 0;
}
