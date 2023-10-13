/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConf.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mouz <ael-mouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/12 15:00:24 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/13 21:44:05 by ael-mouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include "Route.hpp"

class ServerConf
{
private:
	bool Error;
	int Port;
	int NbRoutes;
	int Limit_Client_Body_Size;
	std::string Host;
	std::string Error_Page;
	std::string Server_Name;
	std::string Data;
	Route *route;

public:
	ServerConf();
	ServerConf(std::string data);
	~ServerConf(void);
	void setData(std::string);
	bool getEroor(void);
	int getPort(void);
	int getnb_routes(void);
	int getLimit_Client_Body_Size(void);
	std::string getHost(void);
	std::string getError_Page(void);
	std::string getServer_Name(void);
	Route getRoute(int index);
};