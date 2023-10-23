/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mouz <ael-mouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/12 15:00:24 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/23 18:51:26 by ael-mouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Include.hpp"
#include "MimeTypes.hpp"
#include "Status.hpp"
class ServerConfig
{
public:
    MimeTypes mime;
    Status status;
    int Port;
    std::string Host;
	std::string ErrorPage;
	std::string GlobalRoot;
    std::string GlobalUpload;
    std::string ServerNames;
    std::string LimitClientBodySize;
	std::vector<Route> Routes;
	ServerConfig();
	~ServerConfig();
   const Route *getRoute(const std::string &targetRoutePath) const;
};
