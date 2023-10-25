/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Status.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mouz <ael-mouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/20 20:25:54 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/25 15:51:05 by ael-mouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Status.hpp"

Status::Status()
{
	this->status["100"] = "Continue";
	this->status["101"] = "Switching Protocols";

	this->status["200"] = "OK";
	this->status["201"] = "Created";
	this->status["202"] = "Accepted";
	this->status["203"] = "Non-Authoritative Information";
	this->status["204"] = "No Content";
	this->status["205"] = "Reset Content";
	this->status["206"] = "Partial Content";

	this->status["300"] = "Multiple Choices";
	this->status["301"] = "Moved Permanently";
	this->status["302"] = "Found";
	this->status["303"] = "See Other";
	this->status["304"] = "Not Modified";
	this->status["305"] = "Use Proxy";
	this->status["307"] = "Temporary Redirect";

	this->status["400"] = "Bad Request";
	this->status["401"] = "Unauthorized";
	this->status["402"] = "Payment Required";
	this->status["403"] = "Forbidden";
	this->status["404"] = "Not Found";
	this->status["405"] = "Method Not Allowed";
	this->status["406"] = "Not Acceptable";
	this->status["407"] = "Proxy Authentication Required";
	this->status["408"] = "Request Timeout";
	this->status["409"] = "Conflict";
	this->status["410"] = "Gone";
	this->status["411"] = "Length Required";
    this->status["412"] = "Precondition Failed";
    this->status["413"] = "Request Entity Too Large";
    this->status["414"] = "Request-URI Too Long";
	this->status["415"] = "Unsupported Media Type";
	this->status["416"] = "Requested Range Not Satisfiable";
	this->status["417"] = "Expectation Failed";
    this->status["426"] = "Upgrade Required";

    this->status["500"] = "Internal Server Error";
    this->status["501"] = "Not Implemented";
	this->status["502"] = "Bad Gateway";
	this->status["503"] = "Service Unavailable";
	this->status["504"] = "Gateway Timeout";
	this->status["505"] = "HTTP Version Not Supported";

	// this->status["506"] = "Variant Also Negotiates";
	// this->status["507"] = "Insufficient Storage";
	// this->status["508"] = "Loop Detected";
	// this->status["510"] = "Not Extended";
	// this->status["511"] = "Network Authentication Required";
}

std::string Status::getStatus(const std::string &status) const
{
	std::map<std::string, std::string>::const_iterator it = this->status.find(status);
	if (it != this->status.end())
		return it->second;
	return "Internal Server Error";
}

Status::~Status() {}
