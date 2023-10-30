/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yettabaa <yettabaa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/13 19:36:36 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/30 16:19:29 by yettabaa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Include.hpp"
#include "ServerConfig.hpp"

class Server
{
private:
public:
    ServerConfig DefaultServerConfig;
    std::map<std::string, ServerConfig> OtherServerConfig;
    /**********************/
    // serversocket
    // int listening;
    // vector client
    // std::vector<Client> Clients;
    /**********************/
    // start_server(*this);
    Server(){};
    ~Server(){};
};
