#pragma once
#include "../Utils.hpp"
#include "../Config/MimeTypes.hpp"
#include "../Config/Status.hpp"

class ServerConfig
{
public:
    MimeTypes mime;
    Status status;
    std::string Port;
    std::string Host;
    std::string ServerNames;
	std::string ErrorPage;
	std::string GlobalRoot;
    std::string GlobalUpload;
    std::string LimitClientBodySize;
    std::string phpCgi;
    std::string pythonCgi;
    std::string perlCgi;
    std::string rubyCgi;
	std::vector<Route> Routes;
	ServerConfig();
	~ServerConfig();
   const Route *getRoute(const std::string &targetRoutePath) const;
};
