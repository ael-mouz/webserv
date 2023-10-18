/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mouz <ael-mouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/18 17:01:10 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/18 22:32:31 by ael-mouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <iomanip>
#include <map>
#include <string>
#include <sstream>
class Cgi
{
private:
	std::string responseStatus;
	std::string query;
	std::string path_info;
	std::string script_path;

public:
	void executeCgi();
	void parseUri(std::string uri);
	std::multimap<std::string, std::string> parseHeader(std::string buffer);
};
