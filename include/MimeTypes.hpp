/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MimeTypes.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mouz <ael-mouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/14 22:34:04 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/20 17:44:48 by ael-mouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "Include.hpp"

class MimeTypes
{
private:
	std::map<std::string, std::string> mimeTypes_;

public:
	MimeTypes();
	std::string getMimeType(const std::string &extension) const;
};
