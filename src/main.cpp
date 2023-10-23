/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mouz <ael-mouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/13 19:47:52 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/23 23:07:37 by ael-mouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Include.hpp"
#include "../include/server.hpp"
#include "../include/Config.hpp"

int main(int ac, char **av)
{
	if (ac == 2)
	{
		Config config(av[1]);
        // config.printConfig();
        StartSrever(config.getServerConfig(0));
        return 0;
	}
	return 0;
}
