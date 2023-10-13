#pragma once

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <ctime>
#include <map>
#include <string>
#include <iomanip>
#include <sstream>

enum LogLevel
{
	ERROR,
	WARNING,
	INFO,
	DEBUG
};

void logMessage(LogLevel level, const std::string &message);
std::string convertText(std::string a);