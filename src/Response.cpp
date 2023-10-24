/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-mouz <ael-mouz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/18 16:57:43 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/24 15:53:47 by ael-mouz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Response.hpp"
#include "../include/server.hpp"
#include "../include/ServerConfig.hpp"

void Response::response(int clientSocket, std::string method, std::string uri, std::string httpVersion, std::string Rheaders, std::string body, const ServerConfig &conf)
{
    parseUri(uri);
    if (this->extention != "pl" && this->extention != "py" && this->extention != "php" && this->extention != "rb")
    {
        std::string dir;
        const Route *route = conf.getRoute(this->script_path);
        if (route == NULL)
        {
            if (!this->extention.empty())
                route = conf.getRoute(this->extention);
            if (route == NULL)
            {
                dir = getParentDirectories(this->script_path);
                std::cout << "directory : " << dir << std::endl;
                while (!route && !dir.empty())
                {
                    route = conf.getRoute(dir);
                    dir = getParentDirectories(dir);
                    std::cout << "directory : " << dir << std::endl;
                }
                if (dir.empty() && !route)
                {
                    Route routeee;
                    route = &routeee;
                    std::cout << "no match :" << route->RoutePath << std::endl;
                }
                else
                    std::cout << "directory match : " << route->RoutePath << std::endl;
            }
            else
                std::cout << "extention match: " << route->RoutePath << std::endl;
        }
        else
            std::cout << "full match : " << route->RoutePath << std::endl;
        // if (route->RoutePath == "default")
        // {
        //     generateResponse("404", conf); // TODO : ANOTHER STATUS ERROR OR HANDEL
        //     send(clientSocket, this->responseStatus.c_str(), this->responseStatus.length(), 0);
        //     return;
        // }
        if (route->Root != "default" && route->RoutePath != "default")
            this->script_path = route->Root + this->script_path.substr(route->RoutePath.size());
        else
            this->script_path = conf.GlobalRoot + this->script_path;
        // this->script_path = conf.GlobalRoot + this->script_path;
        std::cout << this->script_path << std::endl;
        // std::ifstream infile(this->script_path);
        // if (!infile.is_open() || this->extention.empty())
        // {
        //     generateResponse("404", conf);
        //     send(clientSocket, this->responseStatus.c_str(), this->responseStatus.length(), 0);
        //     return;
        // }
        // std::cout << "*-------------------------------" << std::endl;
        // std::stringstream bodyStream;
        // bodyStream << infile.rdbuf();
        // std::string body = bodyStream.str();
        // infile.close();
        // this->responseStatus = "HTTP/1.1 200 OK\r\n";
        // this->responseStatus += "Content-Type: " + conf.mime.getMimeType(this->extention) + "\r\n";
        // this->responseStatus += "Content-Length: " + intToString(body.length()) + "\r\n\r\n" + body;
        // ssize_t a = send(clientSocket, this->responseStatus.c_str(), this->responseStatus.length(), 0);
        // std::cout << a << std::endl;
        std::ifstream infile(this->script_path.c_str(), std::ios::binary);
        if (!infile.is_open() || this->extention.empty())
        {
            generateResponse("404", conf);
            send(clientSocket, this->responseStatus.c_str(), this->responseStatus.length(), 0);
            return;
        }
        infile.seekg(0, std::ios::end);
        int fileSize = infile.tellg();
        infile.seekg(0, std::ios::beg);
        std::stringstream header;
        header << "HTTP/1.1 200 OK\r\n";
        header << "Content-Type: " << conf.mime.getMimeType(this->extention) << "\r\n";
        header << "Content-Length: " << fileSize << "\r\n";
        header << "\r\n";
        send(clientSocket, header.str().c_str(), header.str().length(), 0);
        std::cout << "cjdncndjncjnjdncj" << std::endl;
        const int bufferSize = 1024;
        char buffer[bufferSize];
        while (!infile.eof())
        {
            infile.read(buffer, bufferSize);
            ssize_t bytesRead = infile.gcount();
            if (bytesRead > 0)
            {
                ssize_t sentBytes = send(clientSocket, buffer, bytesRead, 0);
                std::cout << sentBytes << std::endl;
                if (sentBytes < 0)
                {
                    generateResponse("500", conf);
                    send(clientSocket, this->responseStatus.c_str(), this->responseStatus.length(), 0);
                    break;
                }
            }
        }
        infile.close();
        return;
    }
    if (method != "GET" && method != "POST" && method != "DELETE")
    {
        generateResponse("501", conf);
        send(clientSocket, this->responseStatus.c_str(), this->responseStatus.length(), 0);
        return;
    }
    else if (httpVersion != "HTTP/1.1\r")
    {
        generateResponse("505", conf);
        send(clientSocket, this->responseStatus.c_str(), this->responseStatus.length(), 0);
        return;
    }
    std::multimap<std::string, std::string> headers = parseHeader(clientSocket, Rheaders, conf); // [x] : parse headers
    headers = mergeHeadersValues(headers);                                                       // [x] : merge duplicated headers
    std::multimap<std::string, std::string> env = generateCGIEnv(headers, method);               // [x] : generate env variable
    char tempFileName[] = "/tmp/.CgitempfileXXXXXXXX";
    int tempFD = -1;
    if (!body.empty())
    {
        tempFD = mkstemp(tempFileName);
        if (tempFD != -1)
        {
            write(tempFD, body.c_str(), body.length()), std::cout << "generate file: " << tempFileName << " fd:" << tempFD << std::endl;
            body.erase();
            lseek(tempFD, 0, SEEK_SET);
        }
        else
        {
            generateResponse("500", conf);
            send(clientSocket, this->responseStatus.c_str(), this->responseStatus.length(), 0);
            return;
        }
    }
    handleCGIScript(clientSocket, method, env, tempFD, conf);
    close(tempFD);        // TODO: CGI DONE
    unlink(tempFileName); // TODO: CGI DONE
}

void Response::parseUri(std::string uri)
{
    size_t posquery = uri.find("?");
    if (posquery != std::string::npos)
    {
        this->query = uri.substr(posquery + 1, uri.length() - posquery);
        uri = uri.substr(0, posquery);
    }
    size_t pos = uri.find_last_of(".");
    if (pos != std::string::npos)
    {
        size_t pos1 = uri.find("/", pos);
        if (pos1 != std::string::npos)
        {
            this->path_info = uri.substr(pos1, uri.length() - pos1);
            this->script_path = uri.substr(0, pos1);
        }
        else
            this->script_path = uri;
    }
    else
        this->script_path = uri;
    if (this->path_info.empty())
        this->path_info = "/";
    pos = this->script_path.find_last_of(".");
    if (pos != std::string::npos)
        this->extention = this->script_path.substr(pos + 1, this->script_path.length() - pos + 1);
    std::cout << "extention: " << extention << std::endl;
    std::cout << "query: " << this->query << std::endl;
    std::cout << "path info: " << this->path_info << std::endl;
    std::cout << "script: " << this->script_path << std::endl;
}

std::multimap<std::string, std::string> Response::parseHeader(int clientSocket, std::string buffer, const ServerConfig &conf)
{
    std::multimap<std::string, std::string> headers;
    std::istringstream stream(buffer);
    std::string line;
    while (std::getline(stream, line))
    {
        if (line.empty() || line == "\r")
            break;
        size_t pos = line.find(':');
        if (pos == std::string::npos)
        {
            generateResponse("400", conf);
            send(clientSocket, this->responseStatus.c_str(), this->responseStatus.length(), 0);
            break;
        }
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 2);
        headers.insert(std::make_pair(key, value));
    }
    // printMap(headers);
    return headers;
}

std::multimap<std::string, std::string> Response::mergeHeadersValues(const std::multimap<std::string, std::string> &headers)
{
    std::multimap<std::string, std::string> newHeaders;
    for (std::multimap<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
    {
        const std::string &key = it->first;
        std::multimap<std::string, std::string>::iterator mergedIt = newHeaders.find(key);
        if (mergedIt == newHeaders.end())
            newHeaders.insert(std::make_pair(key, it->second));
        else
            mergedIt->second += ", " + it->second;
    }
    return newHeaders;
}

std::multimap<std::string, std::string> Response::generateCGIEnv(std::multimap<std::string, std::string> headers, std::string method)
{
    std::multimap<std::string, std::string>::iterator it;
    std::multimap<std::string, std::string>::iterator end;
    std::multimap<std::string, std::string> env;
    //*------------------------------*
    std::string SERVER_SOFTWARE = "webserv";   // MARK : value fix
    std::string SERVER_NAME = "localhost";     // NOTE : my ip address
    std::string GATEWAY_INTERFACE = "CGI/1.1"; // MARK : value fix
    //*------------------------------*
    std::string SERVER_PROTOCOL = "HTTP/1.1"; // MARK : value fix
    std::string SERVER_PORT = "80";           // NOTE : from config
    std::string REQUEST_METHOD = method;
    std::string PATH_INFO = this->path_info;
    // std::string PATH_TRANSLATED = "/mnt/c/Users/SPARROW/Desktop/webserv/www" + this->path_info;	 // NOTE : root + PATH_INFO
    // std::string SCRIPT_NAME = "/mnt/c/Users/SPARROW/Desktop/webserv/www" + this->script_path;	 // NOTE : root + script
    // std::string SCRIPT_FILENAME = "/mnt/c/Users/SPARROW/Desktop/webserv/www" + this->script_path; // NOTE : root +script
    std::string PATH_TRANSLATED = "/Users/ael-mouz/Desktop/webserv/www" + this->path_info;   // NOTE : root + PATH_INFO
    std::string SCRIPT_NAME = "/Users/ael-mouz/Desktop/webserv/www" + this->script_path;     // NOTE : root + script
    std::string SCRIPT_FILENAME = "/Users/ael-mouz/Desktop/webserv/www" + this->script_path; // NOTE : root +script
    std::string QUERY_STRING = this->query;
    std::string CONTENT_TYPE;
    it = headers.find("Content-Type");
    end = headers.end();
    if (it != end)
        CONTENT_TYPE = it->second;
    it = headers.find("Content-Length");
    end = headers.end();
    std::string CONTENT_LENGTH = "0";
    if (it != end)
        CONTENT_LENGTH = it->second;
    // std::string REMOTE_ADDR = "10.12.5.11"; // NOTE:client IP
    //*------------------------------*
    std::string REDIRECT_STATUS = "CGI"; // MARK: for php
    //*------------------------------*
    env.insert(std::make_pair("SERVER_SOFTWARE", SERVER_SOFTWARE));
    env.insert(std::make_pair("SERVER_NAME", SERVER_NAME));
    env.insert(std::make_pair("GATEWAY_INTERFACE", GATEWAY_INTERFACE));
    env.insert(std::make_pair("SERVER_PROTOCOL", SERVER_PROTOCOL));
    env.insert(std::make_pair("SERVER_PORT", SERVER_PORT));
    env.insert(std::make_pair("REQUEST_METHOD", REQUEST_METHOD));
    env.insert(std::make_pair("PATH_INFO", PATH_INFO));
    env.insert(std::make_pair("PATH_TRANSLATED", PATH_TRANSLATED));
    env.insert(std::make_pair("SCRIPT_NAME", SCRIPT_NAME));
    env.insert(std::make_pair("SCRIPT_FILENAME", SCRIPT_FILENAME));
    env.insert(std::make_pair("QUERY_STRING", QUERY_STRING));
    env.insert(std::make_pair("CONTENT_TYPE", CONTENT_TYPE));
    env.insert(std::make_pair("CONTENT_LENGTH", CONTENT_LENGTH));
    // env.insert(std::make_pair("REMOTE_ADDR", REMOTE_ADDR));
    env.insert(std::make_pair("REDIRECT_STATUS", REDIRECT_STATUS)); // NOTE:php only
    it = headers.begin();
    std::string keyEnv, valueEnv;
    for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
    {
        if (it->first == "Content-Type" || it->first == "Content-Length")
            continue;
        std::string keyEnv = "HTTP_" + it->first;
        for (std::string::iterator ch = keyEnv.begin(); ch != keyEnv.end(); ++ch)
            *ch = std::toupper(*ch);
        for (std::string::iterator ch = keyEnv.begin(); ch != keyEnv.end(); ++ch)
        {
            if (*ch == '-')
                *ch = '_';
        }
        std::string valueEnv = it->second;
        env.insert(std::pair<std::string, std::string>(keyEnv, valueEnv));
    }
    printMap(env);
    return env;
}

void Response::handleCGIScript(int clientSocket, const std::string &method, std::multimap<std::string, std::string> env, int tempFD, const ServerConfig &conf)
{
    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
        generateResponse("500", conf);
        send(clientSocket, this->responseStatus.c_str(), this->responseStatus.length(), 0);
        return;
    }
    pid_t pid = fork();
    if (pid == -1)
    {
        generateResponse("500", conf);
        send(clientSocket, this->responseStatus.c_str(), this->responseStatus.length(), 0);
        return;
    }
    if (pid == 0)
    {
        close(pipefd[0]);
        char **envp = new char *[env.size() + 1];
        int i = 0;
        for (std::multimap<std::string, std::string>::iterator it = env.begin(); it != env.end(); it++)
        {
            std::string env_entry = it->first + "=" + it->second;
            envp[i] = new char[env_entry.size() + 1];
            std::strcpy(envp[i], env_entry.c_str());
            i++;
        }
        envp[i] = NULL;
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        alarm(10);
        if (tempFD != -1 && method == "POST")
        {
            dup2(tempFD, STDIN_FILENO);
            close(tempFD);
        }
        if (this->extention == "py")
        {
            char *const args[] = {(char *)"python3", (char *)env.find("SCRIPT_FILENAME")->second.c_str(), NULL};
            execve("/usr/bin/python3", args, envp);
        }
        else if (this->extention == "pl")
        {
            char *const args[] = {(char *)"perl", (char *)env.find("SCRIPT_FILENAME")->second.c_str(), NULL};
            execve("/usr/bin/perl", args, envp);
        }
        else if (this->extention == "rb")
        {
            char *const args[] = {(char *)"ruby", (char *)env.find("SCRIPT_FILENAME")->second.c_str(), NULL};
            execve("/usr/bin/ruby", args, envp);
        }
        else if (this->extention == "php")
        {
            char *const args[] = {(char *)"php-cgi_bin", (char *)env.find("SCRIPT_FILENAME")->second.c_str(), NULL};
            execve("/Users/ael-mouz/Desktop/webserv/tests/php-cgi_bin", args, envp);
        }
        perror("execve failed");
        for (int i = 0; envp[i] != nullptr; i++)
            delete[] envp[i];
        delete[] envp;
        exit(EXIT_FAILURE);
    }
    else
    {
        close(pipefd[1]);
        int status;
        waitpid(pid, &status, 0);
        if (WIFSIGNALED(status) && WTERMSIG(status) == SIGALRM)
        {
            generateResponse("504", conf);
            send(clientSocket, this->responseStatus.c_str(), this->responseStatus.length(), 0);
            return;
        }
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
        {
            char buffer[4096];
            ssize_t bytesRead;
            while ((bytesRead = read(pipefd[0], buffer, 4096)) > 0)
            {
                std::cout << buffer << std::endl;
                send(clientSocket, buffer, bytesRead, 0);
            }
        }
        else
        {
            generateResponse("502", conf);
            send(clientSocket, this->responseStatus.c_str(), this->responseStatus.length(), 0);
            return;
        }
        close(pipefd[0]);
    }
}

void Response::generateResponse(std::string status, const ServerConfig &conf)
{
    std::string extension;
    size_t pos = conf.ErrorPage.find_last_of(".");
    if (pos != std::string::npos)
        extension = conf.ErrorPage.substr(pos + 1, conf.ErrorPage.length() - pos + 1);
    std::ifstream infile(conf.ErrorPage.c_str());
    if (!infile.is_open() || extension.empty())
    {
        std::string body = "<!DOCTYPE html>\n";
        body += "<html lang=\"en\">\n";
        body += "<head>\n<meta charset=\"UTF-8\" />\n";
        body += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" />\n";
        body += "<title>{{Status}} Error - {{Error}}</title>\n";
        body += "<style>\n";
        body += "body {\n";
        body += "margin: 0;\n";
        body += "padding: 0;\n";
        body += "font-family: Arial, sans-serif;\n";
        body += "background-image: url('/images/site42-bg.png');\n";
        body += "background-size: cover;\n";
        body += "background-position: center;\n";
        body += "height: 100vh;\n";
        body += "display: flex;\n";
        body += "flex-direction: column;\n";
        body += "justify-content: center;\n";
        body += "align-items: center;\n";
        body += "color: #000000;\n";
        body += "}\n";
        body += "h1 {\n";
        body += "font-size: 4rem;\n";
        body += "margin-bottom: 20px;\n";
        body += "}\n";
        body += "p {\n";
        body += "font-size: 1.5rem;\n";
        body += "text-align: center;\n";
        body += "}\n";
        body += "</style>\n";
        body += "</head>\n";
        body += "<body>\n";
        body += "<h1>Error {{Status}}</h1>\n";
        body += "<p>{{Error}}</p>\n";
        body += "</body>\n";
        body += "</html>";
        replaceAll(body, "{{Status}}", status);
        replaceAll(body, "{{Error}}", conf.status.getStatus(status));
        this->responseStatus = "HTTP/1.1 " + status + " " + conf.status.getStatus(status) + "\r\n";
        this->responseStatus += "Content-Type: " + conf.mime.getMimeType("html") + "\r\n";
        this->responseStatus += "Content-Length: " + intToString(body.length()) + "\r\n\r\n";
        this->responseStatus += body;
        // std::cout << this->responseStatus << std::endl;
        return;
    }
    std::stringstream bodyStream;
    bodyStream << infile.rdbuf();
    std::cout << extention << std::endl;
    this->responseStatus = "HTTP/1.1 " + status + " " + conf.status.getStatus(status) + "\r\n";
    if (extension == "html" || extension == "htm" || extension == "shtml" ||
        extension == "css" || extension == "xml" || extension == "gif" ||
        extension == "jpeg" || extension == "jpg" || extension == "js" ||
        extension == "atom" || extension == "rss" || extension == "json" ||
        extension == "ico" || extension == "svgz" || extension == "svg" ||
        extension == "pdf" || extension == "doc" || extension == "ppt" ||
        extension == "xls" || extension == "docx" || extension == "xlsx" ||
        extension == "pptx" || extension == "wmlc" || extension == "wasm" ||
        extension == "3gp" || extension == "mp4" || extension == "avi")
        this->responseStatus += "Content-Type: " + conf.mime.getMimeType(extension) + "\r\n";
    else
        this->responseStatus += "Content-Type: " + conf.mime.getMimeType("txt") + "\r\n";
    this->responseStatus += "Content-Length: " + intToString(bodyStream.str().length()) + "\r\n\r\n";
    this->responseStatus += bodyStream.str();
    infile.close();
    // std::cout << this->responseStatus << std::endl;
}
