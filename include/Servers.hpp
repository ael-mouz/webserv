/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Servers.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mouz <ael-mouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/13 19:36:36 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/28 22:20:44 by ael-mouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Include.hpp"
#include "ServerConfig.hpp"

class Servers
{
private:
public:
    ServerConfig DefaultServerConfig;
    std::map<std::string, ServerConfig> OtherServerConfig;
    /**********************/
    // serversocket
    // vector client
    /**********************/
    // start server();
    Servers(){};
    ~Servers(){};
};
