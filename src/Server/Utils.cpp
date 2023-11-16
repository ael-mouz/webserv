#include "../../include/Server/Utils.hpp"
#include "../../include/Server/Client.hpp"

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

void logMessage(LogLevel level, const std::string &host, int fd, const std::string &message)
{
	std::string levelStr;
	std::string color;
	switch (level)
	{
	case SERROR:
		levelStr = "ERROR";
		color = FG_RED;
		break;
	case SCLOSE:
		levelStr = "CLOSE";
		color = FG_RED;
		break;
	case SACCEPT:
		levelStr = "ACCEPT";
		color = FG_GREEN;
		break;
	case SWARNING:
		levelStr = "WARNING";
		color = FG_LYELLOW;
		break;
	case SINFO:
		levelStr = "INFO";
		color = FG_CYAN;
		break;
	case SDEBUG:
		levelStr = "DEBUG";
		color = FG_MAGENTA;
		break;
	case SREQ:
		levelStr = "REQUEST";
		color = FG_BLUE;
		break;
	case SRES:
		levelStr = "RESPONSE";
		color = FG_LBLUE;
		break;
	}
	time_t currentTime = time(NULL);
	struct tm *localTime = localtime(&currentTime);
	char timestamp[20];
	strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localTime);
	std::cout << BOLD << "[" << FG_DGRAY << std::setw(19) << timestamp << FG_WHITE << "] " << RESET_ALL;
	std::cout << BOLD << "( "  << FG_YELLOW<< std::setw(2) << fd <<FG_WHITE << " ) " << RESET_ALL;
	std::cout << BOLD << "[" << FG_DGRAY << host << FG_WHITE << "] " << RESET_ALL;
	std::string level_ = "[" + color + levelStr + FG_WHITE + "]" + RESET_ALL;
	std::cout << BOLD << level_ << std::setfill('=') << std::setw(32 - level_.length()) << ">" << std::setfill(' ');
	std::cout << " { " + message + " }" << std::endl;
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

std::string intToString(size_t number)
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
	if (S_ISDIR(path_stat.st_mode))
		return 1;
	else
		return 2;
}

std::string getParentDirectories(const std::string &uri)
{
	std::string parent;
	size_t pos = uri.find_last_of("/");
	if  (pos == 0)
		parent = "/";
	else if (pos != std::string::npos)
		parent = uri.substr(0, pos);
	return parent;
}

bool isDigit(const std::string &str)
{
	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
	{
		if (!isdigit(*it))
			return false;
	}
	return true;
}

bool isValidURI(const int &c)
{

	if (!isalnum(c) && c != '-' && c != '.' && c != '_' && c != '~' && c != ':' &&
		c != '/' && c != '?' && c != '#' && c != '[' && c != ']' &&
		c != '@' && c != '!' && c != '$' && c != '&' && c != '\'' &&
		c != '(' && c != ')' && c != '*' && c != '+' && c != ',' &&
		c != ';' && c != '=')
	{
		return false;
	}
	return true;
}

bool isValidKey(const int &c) //! #$%&'*+-.^_`|~
{
	if (isalnum(c) || c == '!' || c == '#' || c == '$' || c == '%' ||
		c == '&' || c == '\'' || c == '*' || c == '+' ||
		c == '-' || c == '.' || c == '^' || c == '_' ||
		c == '`' || c == '|' || c == '~')
	{
		return (true);
	}
	return (false);
}

size_t HexaToDicimal(const std::string &hexStr)
{
	std::stringstream converter(hexStr);
	size_t result;
	converter >> std::hex >> result;
	return result;
}

string strToLower(const string& str)
{
    string lower;
    for (string::const_iterator it = str.begin(); it != str.end(); it++)
        lower += std::tolower(*it);
    return lower;
}

string getUploadPath(const Client& client)
{
    std::string path = client.response.route.UploadPath;
    std::string defaultPath = client.response.Config->GlobalUpload;
    if (path == "default")
        return defaultPath;
    return path;
}

bool isRegularFile(const std::string& path)
{
    struct stat fileStat;
    if (stat(path.c_str(), &fileStat) == 0)
        return S_ISREG(fileStat.st_mode);
    return false;
}

bool isDirectory(const std::string& path)
{
    struct stat fileStat;
    if (stat(path.c_str(), &fileStat) == 0)
        return S_ISDIR(fileStat.st_mode);
    return false;
}

bool checkPermission(const std::string path, mode_t permission)
{
    struct stat fileStat;
    if (stat(path.c_str(), &fileStat) == 0)
    {
        return (fileStat.st_mode & permission) != 0;
    }
    return false;
}

void isCanBeRemoved(const std::string& path)
{
    if (isRegularFile(path))
    {
        if (!checkPermission(path, S_IWUSR))
            throw 403;
        return;
    }
    DIR* directory = opendir(path.c_str());
    if ((!checkPermission(path, S_IRUSR) && !checkPermission(path, S_IWUSR)
        && !checkPermission(path, S_IXUSR))|| directory == NULL) {
        throw 403;
    }
    dirent* entry;
    std::string fileName, fullPath;
    while ((entry = readdir(directory)) != NULL) {
        fileName = entry->d_name;
        if (fileName == "." || fileName == "..") {
            continue;
        }
        fullPath = path + "/" + fileName;
        if (isRegularFile(fullPath) && !checkPermission(fullPath, S_IWUSR))
            throw 403;
        else if (isDirectory(fullPath))
            isCanBeRemoved(fullPath);
    }
    closedir(directory);
}

void removeDirfolder(const std::string& path, const std::string& root) //add rout in prototype to not delete it
{
    if (isRegularFile(path))
    {
        std::remove(path.c_str());
        return;
    }
    DIR* directory = opendir(path.c_str());
    if ( directory == NULL) {
        throw 403;
    }
    dirent* entry;
    std::string fileName, fullPath;
    while ((entry = readdir(directory)) != NULL) {
        fileName = entry->d_name;
        if (fileName == "." || fileName == "..") {
            continue;
        }
        fullPath = path + "/" + fileName;
        if (isRegularFile(fullPath)) {
            std::remove(fullPath.c_str());
        }
        else if (isDirectory(fullPath)) {
            removeDirfolder(fullPath, root);
        }
    }
    if (path != root)
        std::remove(path.c_str());
    closedir(directory);
}

bool	timeofday(size_t& timeMilSec)
{
	struct timeval	time;

	if (gettimeofday(&time, NULL) == -1)
        return false;
    timeMilSec = ((time.tv_sec * 1000) + (time.tv_usec / 1000));
	return true;
}

bool getDiskSpace(const string& path, size_t& freeSpace)
{
    struct statvfs stat;
    if (statvfs(path.c_str(), &stat) == 0)
    {
        freeSpace = stat.f_bavail * stat.f_frsize;
        return true;
    }
    return false;
}