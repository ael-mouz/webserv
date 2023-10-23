/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Include.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mouz <ael-mouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/20 17:42:32 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/23 19:46:18 by ael-mouz         ###   ########.fr       */
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
// c headers
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/statvfs.h>
#include <sys/stat.h>

#define BOLD "\e[1m"
#define RESET_ALL "\e[0m"
#define FG_RED "\e[31m"

enum LogLevel
{
    ERROR,
    WARNING,
    INFO,
    DEBUG
};

struct Route
{
    std::string Root;
    std::string Index;
    std::string CgiExec;
    std::string RoutePath;
    std::string Autoindex;
    std::string UploadPath;
    std::string RedirectionURL;
    std::vector<std::string> AcceptedMethods;
    Route() : Root("default"),
              Index("default"),
              CgiExec("default"),
              RoutePath("default"),
              Autoindex("default"),
              UploadPath("default"),
              RedirectionURL("default"){};
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
