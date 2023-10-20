/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mouz <ael-mouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/12 15:00:24 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/20 18:12:14 by ael-mouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Include.hpp"

class ServerConfig
{
public:
	int Port;
	std::string Host;
	std::string ErrorPage;
	std::string GlobalRoot;
	std::string ServerNames;
	std::string LimitClientBodySize;
	std::vector<Route> Routes;
	ServerConfig();
	~ServerConfig();
};
