#include "../../include/Config/Status.hpp"

Status::Status()
{
	this->status["100"] = "Continue";
	this->status["101"] = "Switching Protocols";

	this->status["200"] = "OK";
	this->status["201"] = "Created";
	this->status["202"] = "Accepted";
	this->status["204"] = "No Content";//delete
	this->status["206"] = "Partial Content";//

	this->status["300"] = "Multiple Choices";
	this->status["301"] = "Moved Permanently";
	this->status["302"] = "Found";
	this->status["303"] = "See Other";
	this->status["304"] = "Not Modified";
	this->status["305"] = "Use Proxy";
	this->status["307"] = "Temporary Redirect";

	this->status["400"] = "Bad Request";//+
	this->status["403"] = "Forbidden";//+
	this->status["404"] = "Not Found"; //+
	this->status["405"] = "Method Not Allowed"; //+
	this->status["406"] = "Not Acceptable"; // to search
	this->status["407"] = "Proxy Authentication Required";
	this->status["408"] = "Request Timeout"; //
	this->status["410"] = "Gone";// todo
	this->status["411"] = "Length Required"; //
	this->status["412"] = "Precondition Failed";
	this->status["413"] = "Request Entity Too Large"; //
	this->status["414"] = "Request-URI Too Long"; //
	this->status["415"] = "Unsupported Media Type";
	this->status["416"] = "Requested Range Not Satisfiable";//
	this->status["417"] = "Expectation Failed";// to search

	this->status["500"] = "Internal Server Error";//
	this->status["501"] = "Not Implemented";//
	this->status["502"] = "Bad Gateway";//
	this->status["503"] = "Service Unavailable";
	this->status["504"] = "Gateway Timeout";//
	this->status["505"] = "HTTP Version Not Supported"; //
	this->status["507"] = "Insufficient Storage"; // to do
}

std::string Status::getStatus(const std::string &status) const
{
	std::map<std::string, std::string>::const_iterator it = this->status.find(status);
	if (it != this->status.end())
		return it->second;
	return "Internal Server Error";
}

Status::~Status() {}
