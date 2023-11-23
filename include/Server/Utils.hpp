#pragma once

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
#include <sys/time.h>

class Client;
class ServerConfig;

using namespace std;

#define MAX_URI 4096
#define MAX_KEY 200
#define MAX_VALUE 400
#define MAX_HEADERS 100

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
	Route() : Root("default"),
			  Index("default"),
			  CgiExec("default"),
			  RoutePath("default"),
			  Autoindex("default"),
			  UploadPath("default"),
			  Redirection("default"),
			  RedirectionURL("default"),
			  RedirectionStatus("default"),
			  Accepted_Methods("on"),
			  Accepted_Methods_("GET"),
			  Accepted_Methods__("POST"),
			  Accepted_Methods___("default"){};
};

std::string trim(const std::string &str, const std::string &charactersToTrim);
std::vector<std::string> splitString(const std::string &input, const std::string &delimiter);
void logMessage(LogLevel level, const std::string &host, int fd, const std::string &message);
void printMap(const std::multimap<std::string, std::string> &map);
void replaceAll(std::string &str, const std::string &from, const std::string &to);
std::string intToString(size_t number);
std::string getParentDirectories(const std::string &uri);
int isDirectory(const char *path);
bool isDigit(const std::string &str);
bool isValidURI(const int &c);
bool isValidKey(const int &c);
size_t HexaToDicimal(const std::string &hexStr);
string strToLower(const string &str);
string getUploadPath(const Client &client);
bool isRegularFile(const std::string &path);
bool isDirectory(const std::string &path);
bool checkPermission(const std::string path, mode_t permission);
void isCanBeRemoved(const std::string &path);
void removeDirfolder(const std::string &path, const std::string &root);
bool timeofday(size_t &timeMilSec);
bool getDiskSpace(const string &path, size_t &freeSpace);
std::string getRealPath(std::string path);
std::string ReadableSize(off_t size);
void ft_print_config(int h, ServerConfig &it, bool i);
void ft_print_routes(int h, Route &it, std::string name);
int deleteMthod(Client &client);

#define GENERATE_ERROR_HTML(status, Config)                                            \
	(                                                                                  \
		"<!DOCTYPE html>"                                                              \
		"<html lang=\"en\">"                                                           \
		"<head>"                                                                       \
		"<meta charset=\"UTF-8\" />"                                                   \
		"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" />" \
		"<link rel=\"icon\" type=\"image/x-icon\" href=\"/favicon.ico\">"              \
        "<title>" +                                                                    \
		status +                                                                       \
		" Error - " +                                                                  \
		Config +                                                                       \
		"</title>"                                                                     \
		"<style>"                                                                      \
		"body {"                                                                       \
		"margin: 0;"                                                                   \
		"padding: 0;"                                                                  \
		"font-family: Arial, sans-serif;"                                              \
		"background-image: url('/images/site42-bg.png');"                              \
		"background-size: cover;"                                                      \
		"background-position: contain;"                                                \
		"height: 100vh;"                                                               \
		"display: flex;"                                                               \
		"flex-direction: column;"                                                      \
		"justify-content: center;"                                                     \
		"align-items: center;"                                                         \
		"color: #000000;"                                                              \
		"}"                                                                            \
		"h1 {"                                                                         \
		"font-size: 4rem;"                                                             \
		"margin-bottom: 20px;"                                                         \
		"}"                                                                            \
		"p {"                                                                          \
		"font-size: 1.5rem;"                                                           \
		"text-align: center;"                                                          \
		"}"                                                                            \
		"</style>"                                                                     \
		"</head>"                                                                      \
		"<body>"                                                                       \
		"<h1>Error " +                                                                 \
		status +                                                                       \
		"</h1>"                                                                        \
		"<p>" +                                                                        \
		Config +                                                                       \
		"</p>"                                                                         \
		"</body>"                                                                      \
		"</html>")

#define GENERATE_UPLOAD_HTML(files)                         \
	(                                                       \
		"<!DOCTYPE html>"                                   \
		"<html lang=\"en\">"                                \
		"<head>"                                            \
		"<title>Upload</title>"                             \
        "<link rel=\"icon\" type=\"image/x-icon\" href=\"/favicon.ico\">"              \
		"<style>"                                           \
		"body {"                                            \
		"font-family: Arial, sans-serif;"                   \
		"margin: 0;"                                        \
		"padding: 0;"                                       \
		"background-image: url(\"/images/site42-bg.png\");" \
		"background-size: cover;"                           \
		"background-position: contain;"                     \
		"}"                                                 \
		".upload-status {"                                  \
		"max-width: 600px;"                                 \
		"margin: 20px auto;"                                \
		"padding: 20px;"                                    \
		"background-color: #fff;"                           \
		"box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);"          \
		"text-align: center;"                               \
		"}"                                                 \
		"ul {"                                              \
		"list-style-type: none;"                            \
		"padding: 0;"                                       \
		"}"                                                 \
		"li {"                                              \
		"margin-bottom: 8px;"                               \
		"}"                                                 \
		"</style>"                                          \
		"</head>"                                           \
		"<body>"                                            \
		"<section class=\"upload-status\">"                 \
		"<h2>Status: Created (201)</h2>"                    \
		"<p>The following files were created:</p>"          \
		"<ul>" +                                            \
		files +                                             \
		"</ul>"                                             \
		"</section>"                                        \
		"</body>"                                           \
		"</html>")

#define GENERATE_AUTOINDEX_HTML(fullpath, autoIndexBody)                                          \
	(                                                                                             \
		"<!DOCTYPE html>"                                                                         \
		"<html>"                                                                                  \
		"<head>"                                                                                  \
		"<meta charset=\"UTF-8\">"                                                                \
        "<link rel=\"icon\" type=\"image/x-icon\" href=\"/favicon.ico\">"              \
		"<title>Index of " +                                                                      \
		fullpath +                                                                                \
		"</title>"                                                                                \
		"<style>"                                                                                 \
		"body { font-family: Arial, sans-serif;background-image: url(\"/images/site42-bg.png\");" \
		"background-size: cover;background-position: contain; }"                                  \
		"table { width: 100%; border-collapse: collapse; }"                                       \
		"th, td { padding: 10px; text-align: left; border-bottom: 1px solid black;}"              \
		".icon { width: 20px; height: 20px; margin-right: 10px;object-fit:contain;}"              \
		".Directory { color: #0070c0; background:#f2f2f2; }"                                      \
		".File { color: #000; }"                                                                  \
		".col { text-align: center; }"                                                            \
		"a {text-decoration : none;} "                                                            \
		"</style>"                                                                                \
		"</head>"                                                                                 \
		"<body>"                                                                                  \
		"<table>"                                                                                 \
		"<tr>"                                                                                    \
		"<th>Item</th>"                                                                           \
		"<th class='col'>Type</th>"                                                               \
		"<th class='col'>Size</th>"                                                               \
		"<th class='col'>Permissions</th>"                                                        \
		"<th class='col'>Last access time</th>"                                                   \
		"<th class='col'>Last modification time</th>"                                             \
		"<th class='col'>Last status change time</th>"                                            \
		"</tr>" +                                                                                 \
		autoIndexBody +                                                                           \
		"</table>"                                                                                \
		"</body>"                                                                                 \
		"</html>")
