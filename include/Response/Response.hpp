#pragma once
#include "../Server/Utils.hpp"
#include "../Config/ServerConfig.hpp"

class Response
{
private:
    std::string responseStatus;
    std::string query;
    std::string path_info;
    std::string script_path;
    std::string extention;
    std::string entryPath;
    bool responseDone;
    const Route *route;

public:
    void response(int clientSocket, std::string method, std::string uri, std::string httpVersion, std::string Rheaders, std::string body, const ServerConfig &conf);
    void parseUri(std::string uri);
    std::multimap<std::string, std::string> parseHeader(int clientSocket, std::string buffer, const ServerConfig &conf);
    std::multimap<std::string, std::string> mergeHeadersValues(const std::multimap<std::string, std::string> &headers);
    std::multimap<std::string, std::string> generateCGIEnv(std::multimap<std::string, std::string> headers, std::string method);
    void handleCGIScript(int clientSocket, const std::string &method, std::multimap<std::string, std::string> env, int tempFD, const ServerConfig &conf);
    void generateResponse(std::string status, const ServerConfig &conf);
    void generateAutoIndex(const ServerConfig &conf, std::string &entryPath);
    void getFULLpath(const ServerConfig &conf, int clientSocket);
};
