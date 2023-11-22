#include "../../include/Config/ServerConfig.hpp"

ServerConfig::ServerConfig()
{
	std::string dir;
	const char *homeDir = std::getenv("PWD");
	homeDir != NULL ? dir = homeDir : dir = "";
	this->Port = "8000";
	this->Host = "127.0.0.1";
	this->ServerNames = "webserve";
	this->GlobalRoot = dir + "/www/";
	this->GlobalUpload = dir + "/upload/";
	this->CgiTimeout = "60";
	this->LimitClientBodySize = "10485760";
	this->ErrorPage = dir + "/www/error.html";
	this->phpCgi = dir + "/bin/php-cgi_bin";
	this->pythonCgi = "/usr/bin/python3";
	this->rubyCgi = "/usr/bin/ruby";
	this->perlCgi = "/usr/bin/perl";
}

ServerConfig::~ServerConfig() {}

Route &ServerConfig::getRoute(const std::string &targetRoutePath)
{
	for (std::vector<Route>::iterator it = this->Routes.begin(); it != this->Routes.end(); ++it)
	{
		if (it->RoutePath == targetRoutePath)
			return *it;
	}
	return this->DefaultRoute;
}
