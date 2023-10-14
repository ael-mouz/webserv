/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mouz <ael-mouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/12 15:00:24 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/14 21:57:52 by ael-mouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <vector>

struct Route
{
	std::string Root;
	std::string Index;
	std::string CgiExec;
	std::string RoutePath;
	std::string Autoindex;
	std::string UploadPath;
	std::string RedirectionURL;
	std::vector<std::string> AcceptedMethods;
	Route() : Root("default"),
			  Index("default"),
			  CgiExec("default"),
			  RoutePath("default"),
			  Autoindex("default"),
			  UploadPath("default"),
			  RedirectionURL("default"){};
};

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
