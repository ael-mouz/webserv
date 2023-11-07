#pragma once

#include "../Config/ServerConf.hpp"
#include "../Config/ServerConfig.hpp"
#include "../Server/Utils.hpp"

class Client;

class Response
{
private:
public:
	std::string responseString;
	std::string HeaderResponse;
	std::string BodyResponse;
	std::string query;
	std::string path_info;
	std::string path_translated;
	std::string fullpath;
	std::string extension;
	std::string entryPath;
	bool closeClient;
	bool isBodySent;
	bool isHeaderSent;
	bool isCgi;
	bool responseDone;
	bool responseSent;
	int fd;
	FILE *fptr;
	size_t fileSize;
	size_t offset;
	int match;
	Route route;
	ServerConfig *Config;
	std::multimap<std::string, std::string> env;

	Response(void);
	~Response(void);
	void getRoute(void);
	void response(Client &client);
	void clear(void);
	void genrateRederiction();
	void mergeHeadersValues(Client &client);
	void getConfig(Client &client);
	void parseUri(std::string uri);
	void getFULLpath(void);
	void regenerateExtonsion();
	void handleCGIScript(Client &client);
	void handleNormalFiles(Client &client);
	void handleRange(stringstream &header,const std::string &range);
	void generateCGIEnv(Client &client);
	void generateResponse(std::string status);
	void generateAutoIndex(void);
	void sendResponse(Client &client);
	std::multimap<std::string, std::string> parseResponseHeader(std::string header);
	std::string generateResponseHeaderCGI(std::multimap<std::string, std::string> &headers, std::string &body);
	// std::string buffer);
};
