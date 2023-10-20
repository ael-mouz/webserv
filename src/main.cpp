/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mouz <ael-mouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/13 19:47:52 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/20 18:57:05 by ael-mouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Include.hpp"
#include "../include/server.hpp"
#include "../include/Config.hpp"
#include "../include/MimeTypes.hpp"

int main(int ac, char **av)
{
	if (ac == 2)
	{
		Config config(av[1]);
		MimeTypes mimetypes;
		std::cout << mimetypes.getMimeType("html") <<  std::endl;
		StartSrever();
		return 0;
	}
	return 0;
}
