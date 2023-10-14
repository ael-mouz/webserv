/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mouz <ael-mouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/13 19:36:36 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/13 19:37:41 by ael-mouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>

class Route
{
private:
	bool error;
	std::string Root;
	std::string Index;
	std::string Location;
	std::string Cgi_Exec;
	std::string Autoindex;
	std::string Upload_Path;
	std::string Redirection[2];
	std::string Accepted_Method[3];

public:
	Route(std::string data);
	~Route(void);
	bool getEroor(void);
	std::string getRoot();
	std::string getIndex();
	std::string getLocation();
	std::string getCgi_Exec();
	std::string getAutoindex();
	std::string getUpload_Path();
	std::string getRedirection();
	std::string getAccepted_Method(std::string method);
};