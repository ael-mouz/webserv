/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mouz <ael-mouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/14 22:36:08 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/23 23:06:32 by ael-mouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ServerConfig.hpp"

ServerConfig::ServerConfig()
{
    std::string dir;
    const char *homeDir = std::getenv("PWD");
    homeDir != NULL ? dir = homeDir : dir = "";
    Port = 80;
    Host = "127.0.0.1";
    ServerNames = "webserver";
    GlobalRoot = dir + "/www";
    GlobalUpload = dir + "/upload";
    LimitClientBodySize = "10M";
    ErrorPage = "default";
}

ServerConfig::~ServerConfig() {}

const Route *ServerConfig::getRoute(const std::string &targetRoutePath) const
{
    const Route *route;
    for (std::vector<Route>::const_iterator it = this->Routes.begin(); it != this->Routes.end(); ++it)
    {
        if (it->RoutePath == targetRoutePath)
        {
            route = &(*it);
            return route;
        }
    }
    return NULL;
}
