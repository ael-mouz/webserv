/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mouz <ael-mouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/13 19:47:52 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/25 22:29:21 by ael-mouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Include.hpp"
#include "../include/server.hpp"
#include "../include/Config.hpp"

int main(int ac, char **av)
{
    if (ac == 2)
    {
        Config config;
        config.parser(av[1]);
#ifdef DEBUG_C
        config.printConfig();
#endif
        StartSrever(config.getServerConfig(0));
        return 0;
    }
    return 0;
}
