#pragma once

#include "../Config/ServerConf.hpp"
#include "../Config/ServerConfig.hpp"
#include "../Server/Utils.hpp"

class Client;

class Response
{
public:
	bool CgiRunning;
	bool headerMerged;
	bool headerCgiReady;
	bool envCgiReady;
	pid_t pid;
	int tempFD;
	int FDCGIBody;
	int pipefd[2];
	std::string resCgi;
	std::string tempFileName;
	std::multimap<std::string, std::string> MAPhederscgi;
public:
	bool isCgi;
	bool isBodySent;
	bool closeClient;
	bool isHeaderSent;
	bool responseDone;
	bool responseSent;
	bool method_allowd;
	bool head_method;
	bool error_page;
	int match;
	size_t offset;
	size_t fileSize;
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
	std::string root;
	std::multimap<std::string, std::string> env;
	Route route;
	FILE *fptr;
	ServerConfig *Config;

	Response(void);
	~Response(void);
	void startResponse(Client &client);
	void checkErrorsRequest(Client &client);
	void CGI(Client &client);
	void checkerPath(Client &clinet);
	void getConfig(Client &client);
	void parseUri(std::string uri);
	void getFULLpath(void);
	void getRoute(void);
	void checkAcceptedMethod(Client &client);
	void genrateRederiction(Client &client);
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
	std::string generateResponseHeaderCGI(std::multimap<std::string, std::string> &headers,size_t body_lenght,std::string Rstatus);
	// std::string buffer);
};
