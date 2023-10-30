#pragma once
#include "../Utils.hpp"
#include "../Config/ServerConfig.hpp"
#include "../Config/ServerConf.hpp"

class Config
{
private:
    int NbServer;
    std::vector<ServerConfig> Serverconfig;
	std::vector<ServerConf> Servers_;

public:
    Config(void);
    ~Config(void);
    void parser(std::string filename);
    void parseServer(const std::string &data, ServerConfig &serverConfig, int start, const std::string &filename);
    void parseRoute(const std::string &data, Route &route, int start, const std::string &filename);
    void parseBodySize(std::string &sizeStr, int start, const std::string &line, const std::string &filename);
    void printConfig(void);
    void printServers(void);
    /*checker*/
    void parseMethods(Route &route, int start, const std::string &line, const std::string &filename);
    void parsePort(std::string &port, int start, const std::string &line, const std::string &filename);
    void parseRedirection(Route &route, int start, const std::string &line, const std::string &filename);
    void parsePath(const std::string &path, int start, const std::string &line, const std::string &filename);
    void parseRoutePath(const std::string &path, int start, const std::string &line, const std::string &filename);
    void parseAutoindex(const std::string &value, int start, const std::string &line, const std::string &filename);
    void parseServerNameAndHostName(std::string &serverName, int start, const std::string &line, const std::string &filename,int h);
    void getIpv4Address(std::string &hostname, int start, const std::string &line, const std::string &filename);
    void checkServerConfig(const std::string &filename);
    void filterServerConfig();
    /*getter and setter*/
    int getNbServer(void) const;
    // Server getServerConfig(size_t index) const;
	std::vector<ServerConf> &getServerConfig();
};
