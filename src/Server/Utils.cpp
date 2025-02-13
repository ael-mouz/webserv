#include "../../include/Server/Utils.hpp"
#include "../../include/Server/Client.hpp"
#include "../../include/Config/ServerConfig.hpp"

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
	std::cout << BOLD << "( " << FG_YELLOW << std::setw(2) << fd << FG_WHITE << " ) " << RESET_ALL;
	std::cout << BOLD << "[" << FG_DGRAY << host << FG_WHITE << "] " << RESET_ALL;
	std::string level_ = "[" + color + levelStr + FG_WHITE + "]" + RESET_ALL;
	std::cout << BOLD << level_ << std::setfill('=') << std::setw(32 - level_.length()) << ">" << std::setfill(' ');
	std::cout << " { " + message + " }" << std::endl;
}

std::string intToString(size_t number)
{
	std::ostringstream oss;
	oss << number;
	return oss.str();
}

std::string getParentDirectories(const std::string &uri)
{
	std::string parent;
	size_t pos = uri.find_last_of("/");
	if (pos == 0)
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

bool isValidKey(const int &c)
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

string strToLower(const string &str)
{
	string lower;
	for (string::const_iterator it = str.begin(); it != str.end(); it++)
		lower += std::tolower(*it);
	return lower;
}

string getUploadPath(const Client &client)
{
	std::string path = client.response.route.UploadPath;
	std::string defaultPath = client.response.Config->GlobalUpload;
	if (path == "default")
		return defaultPath;
	return path;
}

bool isRegularFile(const std::string &path)
{
	struct stat fileStat;
	if (stat(path.c_str(), &fileStat) == 0)
		return S_ISREG(fileStat.st_mode);
	return false;
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

bool isDirectory(const std::string &path)
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
		return (fileStat.st_mode & permission) != 0;
	return false;
}

void isCanBeRemoved(const std::string &path)
{
	if (access(path.c_str(), F_OK) != 0)
		throw 404;
	if (isRegularFile(path))
	{
		if (!checkPermission(path, S_IWUSR))
			throw 403;
		return;
	}
	DIR *directory = opendir(path.c_str());
	if ((!checkPermission(path, S_IRUSR) && !checkPermission(path, S_IWUSR) && !checkPermission(path, S_IXUSR)) || directory == NULL)
		throw 403;
	dirent *entry;
	std::string fileName, fullPath;
	while ((entry = readdir(directory)) != NULL)
	{
		fileName = entry->d_name;
		if (fileName == "." || fileName == "..")
			continue;
		fullPath = path + "/" + fileName;
		if (isRegularFile(fullPath) && !checkPermission(fullPath, S_IWUSR))
			throw 403;
		else if (isDirectory(fullPath))
			isCanBeRemoved(fullPath);
	}
	closedir(directory);
}

void removeDirfolder(const std::string &path, const std::string &root)
{
	if (isRegularFile(path))
		return (std::remove(path.c_str()), void());
	DIR *directory = opendir(path.c_str());
	if (directory == NULL)
		throw 403;
	dirent *entry;
	std::string fileName, fullPath;
	while ((entry = readdir(directory)) != NULL)
	{
		fileName = entry->d_name;
		if (fileName == "." || fileName == "..")
			continue;
		fullPath = path + "/" + fileName;
		if (isRegularFile(fullPath))
			std::remove(fullPath.c_str());
		else if (isDirectory(fullPath))
			removeDirfolder(fullPath, root);
	}
	if (path != root)
		std::remove(path.c_str());
	closedir(directory);
}

int deleteMthod(Client &client)
{
	try
	{
		std::string root = client.response.root;
		isCanBeRemoved(client.response.fullpath);
		removeDirfolder(client.response.fullpath, root);
	}
	catch (int returnValue)
	{
		client.request.setErrorMsg("");
		return returnValue;
	}
	return 200;
}

bool timeofday(size_t &timeMilSec)
{
	struct timeval time;

	if (gettimeofday(&time, NULL) == -1)
		return false;
	timeMilSec = ((time.tv_sec * 1000) + (time.tv_usec / 1000));
	return true;
}

bool getDiskSpace(const string &path, size_t &freeSpace)
{
	struct statvfs stat;
	if (statvfs(path.c_str(), &stat) == 0)
	{
		freeSpace = stat.f_bavail * stat.f_frsize;
		return true;
	}
	return false;
}

std::string getRealPath(std::string path)
{
	char buf[4096];
	char *res = realpath(path.c_str(), buf);
	if (res)
		return std::string(buf);
	else
		return path;
}

std::string ReadableSize(off_t size)
{
	std::ostringstream sizeStr;
	if (size >= 1073741824)
		sizeStr << (size / 1073741824) << " GB";
	else if (size >= 1048576)
		sizeStr << (size / 1048576) << " MB";
	else if (size >= 1024)
		sizeStr << (size / 1024) << " KB";
	else
		sizeStr << size << " B";
	return sizeStr.str();
}

void printMap(const std::multimap<std::string, std::string> &map)
{
	std::cout << "╔═════════════════════════════════╦══════════════════════════════════════════════════════════════════╗" << std::endl;
	std::cout << "║ Key:                            ║ Value:                                                           ║" << std::endl;
	std::cout << "╠═════════════════════════════════╬══════════════════════════════════════════════════════════════════╣" << std::endl;
	std::map<std::string, std::string>::const_iterator it1 = map.begin();
	for (; it1 != map.end(); ++it1)
		std::cout << "║" << std::setw(37) << ("▻" + it1->first + "◅") << "║" << std::setw(70) << ("▻" + it1->second + "◅") << "║" << std::endl;
	std::cout << "╚═════════════════════════════════╩══════════════════════════════════════════════════════════════════╝" << std::endl;
}

void ft_print_config(int h, ServerConfig &it, bool i)
{
	std::cout << "▻" BOLD " " << it.ServerNames << " " RESET_ALL "◅" << std::endl;
	std::cout << "╔═══════════════════════════════════════════════════════════════════════════╗" << std::endl;
	if (i)
		std::cout << "║" FG_BLUE BOLD " DEFAULT CONFIG " RESET_ALL << h << std::setw(61) << "║" << std::endl;
	else
		std::cout << "║" BOLD " OTHER CONFIG " RESET_ALL << h << std::setw(63) << "║" << std::endl;
	std::cout << "╠═════════════════╦═════════════════════════════════════════════════════════╣" << std::endl;
	std::cout << "║ Port            ║" << std::setw(58) << "▻" + it.Port << "◅║" << std::endl;
	std::cout << "║ Host            ║" << std::setw(58) << "▻" + it.Host << "◅║" << std::endl;
	std::cout << "║ ServerNames     ║" << std::setw(58) << "▻" + it.ServerNames << "◅║" << std::endl;
	std::cout << "║ ErrorPage       ║" << std::setw(58) << "▻" + it.ErrorPage << "◅║" << std::endl;
	std::cout << "║ LimitBodySize   ║" << std::setw(58) << "▻" + it.LimitClientBodySize << "◅║" << std::endl;
	std::cout << "║ CgiTimeout      ║" << std::setw(58) << "▻" + it.CgiTimeout << "◅║" << std::endl;
	std::cout << "║ GlobalRoot      ║" << std::setw(58) << "▻" + it.GlobalRoot << "◅║" << std::endl;
	std::cout << "╚═════════════════╩═════════════════════════════════════════════════════════╝" << std::endl;
}

void ft_print_routes(int h, Route &it, std::string name)
{
	std::cout << "↦  ▻" BOLD " " << name << " " RESET_ALL "◅" << std::endl;
	std::cout << "↦  ╔═══════════════════════════════════════════════════════════════════════════╗" << std::endl;
	std::cout << "↦  ║" FG_BLUE BOLD " ROUTE " << h << RESET_ALL << std::setw(70) << "║" << std::endl;
	std::cout << "↦  ╠═════════════════╦═════════════════════════════════════════════════════════╣\n";
	std::cout << "↦  ║ RoutePath       ║" << std::setw(58) << "▻" + it.RoutePath << "◅║" << std::endl;
	std::cout << "↦  ║ UploadPath      ║" << std::setw(58) << "▻" + it.UploadPath << "◅║" << std::endl;
	std::cout << "↦  ║ Redirection     ║" << std::setw(58) << "▻" + it.Redirection << "◅║" << std::endl;
	std::cout << "↦  ║ RedirectStatus  ║" << std::setw(58) << "▻" + it.RedirectionStatus << "◅║" << std::endl;
	std::cout << "↦  ║ RedirectionURL  ║" << std::setw(58) << "▻" + it.RedirectionURL << "◅║" << std::endl;
	std::cout << "↦  ║ Root            ║" << std::setw(58) << "▻" + it.Root << "◅║" << std::endl;
	std::cout << "↦  ║ Autoindex       ║" << std::setw(58) << "▻" + it.Autoindex << "◅║" << std::endl;
	std::cout << "↦  ║ Index           ║" << std::setw(58) << "▻" + it.Index << "◅║" << std::endl;
	std::cout << "↦  ║ Cgi_Exec        ║" << std::setw(58) << "▻" + it.CgiExec << "◅║" << std::endl;
	std::cout << "↦  ║ Methods         ║" << std::setw(58) << "▻" + it.Accepted_Methods << "◅║" << std::endl;
	std::cout << "↦  ║ Methods1        ║" << std::setw(58) << "▻" + it.Accepted_Methods_ << "◅║" << std::endl;
	std::cout << "↦  ║ Methods2        ║" << std::setw(58) << "▻" + it.Accepted_Methods__ << "◅║" << std::endl;
	std::cout << "↦  ║ Methods3        ║" << std::setw(58) << "▻" + it.Accepted_Methods___ << "◅║" << std::endl;
	std::cout << "↦  ╚═════════════════╩═════════════════════════════════════════════════════════╝" << std::endl;
};
