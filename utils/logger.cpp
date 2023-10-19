#include "../include/server.hpp"

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