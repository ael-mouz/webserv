/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mouz <ael-mouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/20 18:45:00 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/28 14:36:39 by ael-mouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Include.hpp"

std::string trim(const std::string &str, const std::string &charactersToTrim)
{
    if (str.empty())
        return str;
    size_t start = str.find_first_not_of(charactersToTrim);
    if (start == std::string::npos)
        return "";
    size_t end = str.find_last_not_of(charactersToTrim);
    return str.substr(start, end - start + 1);
}

std::vector<std::string> splitString(const std::string &input, const std::string &delimiter)
{
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = input.find(delimiter);
    while (end != std::string::npos)
    {
        result.push_back(input.substr(start, end - start));
        start = end + delimiter.size();
        end = input.find(delimiter, start);
    }
    result.push_back(input.substr(start, end));
    return result;
}

std::string convertText(std::string a)
{
    std::string::size_type i = 0;
    std::string newbuff;
    while (i < a.length())
    {
        if (a[i] == '\r' && i + 1 < a.length() && a[i + 1] == '\n')
        {
            newbuff += "\e[41m\\r\\n\e[49m\n";
            i++;
        }
        else if (a[i] == '\t')
            newbuff += "\\t\t";
        else if (a[i] == '\v')
            newbuff += "\\v\v";
        else if (a[i] == '\f')
            newbuff += "\\f\f";
        else
            newbuff += a[i];
        i++;
    }
    return newbuff;
}

void logMessage(LogLevel level, const std::string &message)
{
    std::string levelStr;
    switch (level)
    {
    case ERROR:
        levelStr = "ERROR";
        break;
    case WARNING:
        levelStr = "WARNING";
        break;
    case INFO:
        levelStr = "INFO";
        break;
    case DEBUG:
        levelStr = "DEBUG";
        break;
    }
    time_t currentTime = time(NULL);
    struct tm *localTime = localtime(&currentTime);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localTime);
    std::cout << "[" << timestamp << "] [" << levelStr << "] " << message << std::endl;
}

void printMap(const std::multimap<std::string, std::string> &map)
{
    std::cout << std::setfill('-') << std::setw(145) << "-" << std::endl;
    std::map<std::string, std::string>::const_iterator it1 = map.begin();
    for (; it1 != map.end(); ++it1)
        std::cout << "|Key:" << std::setfill(' ') << std::setw(30) << it1->first << "| Value: " << std::setw(100) << it1->second << "|" << std::endl;
    std::cout << std::setfill('-') << std::setw(145) << "-" << std::endl;
}

size_t getFreeSpace(const char *path)
{
    struct statvfs buf;
    size_t free_space_bytes;
    if (statvfs(path, &buf) == 0)
    {
        free_space_bytes = buf.f_frsize * buf.f_bavail;
        return free_space_bytes;
    }
    else
        return -1;
}

void replaceAll(std::string &str, const std::string &from, const std::string &to)
{
    size_t startPos = 0;
    while ((startPos = str.find(from, startPos)) != std::string::npos)
    {
        str.replace(startPos, from.length(), to);
        startPos += to.length();
    }
}

std::string intToString(int number)
{
    std::ostringstream oss;
    oss << number;
    return oss.str();
}

int isDirectory(const char *path)
{
    struct stat path_stat;
    if (stat(path, &path_stat) != 0)
        return -1;
    if(S_ISDIR(path_stat.st_mode))
        return 1;
    else
        return 2;
}

std::string getParentDirectories(const std::string &uri)
{
    std::string parent;
    size_t pos = uri.find_last_of("/");
    if (pos != std::string::npos)
        parent = uri.substr(0, pos);
    return parent;
}

bool isStringDigits(const std::string &str)
{
    for (size_t i = 0; i < str.length(); ++i)
    {
        if (!isdigit(str[i]))
            return false;
    }
    return true;
}
