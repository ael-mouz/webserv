#include "../../include/Server/Client.hpp"

Client::Client(ServerConf &serverConf, int socketClient, std::string clientIP,std::string host)
	: socketClient(socketClient), serverConf(serverConf)
{
	this->clientHost = host;
	this->clientIP = clientIP;
	read = true;
	write = false;
}

Client &Client::operator=(const Client &overl)
{
	clientHost = overl.clientHost;
	clientIP = overl.clientIP;
	socketClient = overl.socketClient;
	request = overl.request;
	response = overl.response;
	read = overl.read;
	write = overl.write;
	return (*this);
}

Client::Client(const Client &copy)
	: serverConf(copy.serverConf)
{
	*this = copy;
}

Client::~Client() {}
