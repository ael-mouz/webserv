#pragma once

#include "../Config/ServerConf.hpp"
#include "../Config/ServerConfig.hpp"
#include "../Server/Utils.hpp"
// #include "../Server/Client.hpp"

class Client;

class Response
{
private:
public:
	std::string responseStatus;
	std::string HeaderResponse;
	std::string BodyResponse;
	std::string query;
	std::string path_info;
	std::string path_translated;
	std::string fullpath;
	std::string extension;
	std::string entryPath;
	bool isBodySent;
	bool isHeaderSent;
	bool isCgi;
	bool responseDone;
	bool responseSent;
	std::ifstream infile;
	size_t fileSize;
	size_t ofsset;
	Route *route;
	ServerConf &serverConf;
	ServerConfig *Config;
	std::multimap<std::string, std::string> env;

	Response(ServerConf &serverConf);
	~Response(void);
	/*add this in request ???? */
	void response(Client &client);
	void clear();
	void mergeHeadersValues(Client &client);
	void getConfig(Client &client);
	void parseUri(std::string uri);
	void getFULLpath(void);
	void regenerateExtonsion();
	void handleCGIScript(Client &client);
	void handleNormalFiles(void);
	void generateCGIEnv(Client &client);
	void generateResponse(std::string status);
	void generateAutoIndex(void);
	void sendResponse(Client &client);
	// std::multimap<std::string, std::string> parseHeader(int clientSocket,
	// std::string buffer);
};
