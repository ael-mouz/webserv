/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mouz <ael-mouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/14 22:36:08 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/16 20:15:54 by ael-mouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ServerConfig.hpp"

ServerConfig::ServerConfig()
{
	std::string dir;
	const char *homeDir = std::getenv("HOME");
	homeDir != NULL ? dir = homeDir : dir = "";
	Port = 80;
	Host = "127.0.0.1";
	ServerNames = "webserver";
	GlobalRoot = dir + "/www/";
	LimitClientBodySize = "10M";
	ErrorPage = dir + "/www/error/error.html";
}

ServerConfig::~ServerConfig()
{

}