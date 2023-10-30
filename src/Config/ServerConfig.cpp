/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mouz <ael-mouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/14 22:36:08 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/30 19:37:18 by ael-mouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../Include/Config/ServerConfig.hpp"

ServerConfig::ServerConfig()
{
    std::string dir;
    const char *homeDir = std::getenv("PWD");
    homeDir != NULL ? dir = homeDir : dir = "";
    this->Port = 80;
    this->Host = "127.0.0.1";
    this->ServerNames = "webserver";
    this->GlobalRoot = dir + "/www";
    this->GlobalUpload = dir + "/upload";
    this->LimitClientBodySize = "10M";
    this->ErrorPage = "default";
    this->phpCgi = dir + "/tests/php-cgi_bin";
    this->pythonCgi = "/usr/bin/python3";
    this->rubyCgi = "/usr/bin/ruby";
    this->perlCgi = "/usr/bin/perl";
}

ServerConfig::~ServerConfig() {}

const Route *ServerConfig::getRoute(const std::string &targetRoutePath) const
{
    for (std::vector<Route>::const_iterator it = this->Routes.begin(); it != this->Routes.end(); ++it)
    {
        if (it->RoutePath == targetRoutePath)
            return &(*it);
    }
    return NULL;
}
