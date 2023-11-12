#pragma once

#include "../Config/ServerConf.hpp"
#include "../Config/ServerConfig.hpp"
#include "../Server/Utils.hpp"

class Client;

class Response
{
private:
public:
	bool CgiRunning;
	int pipefd[2];
	bool headerCgiReady;
	std::string resCgi;
	int tempFD;
	int FDCGIBody;
	std::string tempFileName;
	std::multimap<std::string, std::string> MAPhederscgi;
	pid_t pid;
	std::string responseStatus;
	std::string responseString;
	std::string HeaderResponse;
	std::string BodyResponse;
	std::string query;
	std::string path_info;
	std::string path_translated;
	std::string fullpath;
	std::string extension;
	std::string entryPath;
    bool method_allowd;
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
	void startResponse(Client &client);
	void checkErrorsRequest(Client &client);
	void CGI(Client &client);
	void checkerPath();
	void getConfig(Client &client);
	void parseUri(std::string uri);
	void getFULLpath(void);
	void getRoute(void);
	void genrateRederiction();
	void regenerateExtonsion();
	void clear(void);
	void handleNormalFiles(Client &client);
	void handleRange(stringstream &header, const std::string &range);
	void handleScriptCGI(Client &client);
	void mergeHeadersValuesCGI(Client &client);
	void generateEnvCGI(Client &client);
	void generateResponse(std::string status);
	void generateAutoIndex(void);
	void sendResponse(Client &client);
	void ft_printroute();
	void ft_printconfig();
	std::multimap<std::string, std::string> parseResponseHeader(std::string header);
	std::string generateResponseHeaderCGI(std::multimap<std::string, std::string> &headers,size_t body_lenght);
	// std::string buffer);
};
