#pragma once

// cpp headers
#include <set>
#include <map>
#include <ctime>
#include <fstream>
#include <vector>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <iomanip>
#include <string>
#include <sstream>
#include <numeric>
#include <utility>
// c headers
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/statvfs.h>
#include <sys/stat.h>
#include <dirent.h>
#include <netdb.h>
#include <netinet/in.h>

using namespace std;

#define MAX_URI 4096
#define MAX_KEY 200
#define MAX_VALUE 400

#define BOLD "\e[1m"
#define DIM "\e[2m"
#define UNDERLINED "\e[4m"
#define BLINK "\e[5m"
#define REVERSE "\e[7m"
#define HIDDEN "\e[8m"

#define RESET_ALL "\e[0m"

#define FG_DEFAULT "\e[39m"
#define FG_BLACK "\e[30m"
#define FG_RED "\e[31m"
#define FG_GREEN "\e[32m"
#define FG_YELLOW "\e[33m"
#define FG_BLUE "\e[34m"
#define FG_MAGENTA "\e[35m"
#define FG_CYAN "\e[36m"
#define FG_LGRAY "\e[37m"
#define FG_DGRAY "\e[90m"
#define FG_LRED "\e[91m"
#define FG_LGREEN "\e[92m"
#define FG_LYELLOW "\e[93m"
#define FG_LBLUE "\e[94m"
#define FG_LMAGENTA "\e[95m"
#define FG_LCYAN "\e[96m"
#define FG_WHITE "\e[97m"

enum LogLevel
{
	SERROR,
	SWARNING,
	SINFO,
	SDEBUG,
	SREQ,
	SRES,
	SACCEPT,
	SCLOSE
};

struct Route
{
	std::string Root;
	std::string Index;
	std::string CgiExec;
	std::string RoutePath;
	std::string Autoindex;
	std::string UploadPath;
	std::string Redirection;
	std::string RedirectionURL;
	std::string RedirectionStatus;
	std::string Accepted_Methods;
	std::string Accepted_Methods_;
	std::string Accepted_Methods__;
	std::string Accepted_Methods___;
	// std::vector<std::string> AcceptedMethods;
	Route() : Root("default"),
			  Index("default"),
			  CgiExec("default"),
			  RoutePath("default"),
			  Autoindex("default"),
			  UploadPath("default"),
			  Redirection("default"),
			  RedirectionURL("default"),
			  RedirectionStatus("default"),
			  Accepted_Methods("default"),
			  Accepted_Methods_("default"),
			  Accepted_Methods__("default"),
			  Accepted_Methods___("default"){};
};

std::string trim(const std::string &str, const std::string &charactersToTrim);
std::vector<std::string> splitString(const std::string &input, const std::string &delimiter);
std::string convertText(std::string a);
// void logMessage(LogLevel level, const std::string &message);
void logMessage(LogLevel level, const std::string &host, int fd, const std::string &message);
void printMap(const std::multimap<std::string, std::string> &map);
size_t getFreeSpace(const char *path);
void replaceAll(std::string &str, const std::string &from, const std::string &to);
std::string intToString(size_t number);
std::string getParentDirectories(const std::string &uri);
int isDirectory(const char *path);
bool isDigit(const std::string &str);
bool ValidURI(const int &c);
bool ValidKey(const int &c);
size_t HexaToDicimal(const std::string &hexStr);
