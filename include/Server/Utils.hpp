/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yettabaa <yettabaa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/20 17:42:32 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/11/01 13:14:55 by yettabaa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
#define RESET_ALL "\e[0m"
#define FG_RED "\e[31m"
#define FG_BLUE "\e[34m"

enum LogLevel
{
    SERROR,
    SWARNING,
    SINFO,
    SDEBUG
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
void logMessage(LogLevel level, const std::string &message);
void printMap(const std::multimap<std::string, std::string> &map);
size_t getFreeSpace(const char *path);
void replaceAll(std::string &str, const std::string &from, const std::string &to);
std::string intToString(int number);
std::string getParentDirectories(const std::string &uri);
int isDirectory(const char *path);
bool isDigit(const std::string &str);
bool ValidURI(const int &c);
bool ValidKey(const int &c);
size_t HexaToDicimal(const std::string& hexStr);
