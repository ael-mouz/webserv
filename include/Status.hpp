/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Status.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mouz <ael-mouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/20 20:22:26 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/20 22:00:55 by ael-mouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Include.hpp"

class Status
{
private:
	std::map<std::string, std::string> status;

public:
	Status();
	~Status();
	std::string getStatus(const std::string &status) const;
};