/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mouz <ael-mouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/20 17:48:55 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/20 18:13:16 by ael-mouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Include.hpp"

void parseRequest(const std::string &request, std::string &method, std::string &uri, std::string &httpVersion, std::string &headers, std::string &body);
std::string receiveRequest(int clientSocket);
void StartSrever();