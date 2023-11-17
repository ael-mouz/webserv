#include "../../include//Server/RunServers.hpp"

#define forever for (;;)

void RunServers::runing()
{
	recvbuffer = new char[4096 * 4]; // delete it
	forever
	{
		resetFds();
		numberOfEvents = select(maxFds + 1, &readFds, &writeFds, NULL, &timeout);
		if (numberOfEvents <= -1)
		{
			std::cout << "Error in select function\n"; ///!!
		}
		else if (numberOfEvents == 0)
            timeoutChecker();
		else
		{
			for (vector<Client>::iterator it = clients.begin(); it != clients.end();)
			{
                timeoutClientChecker(*it);
				if (FD_ISSET(it->socketClient, &readFds))
				{
					if (receiveData(it) == false)
                        continue;
				}
				else if (FD_ISSET(it->socketClient, &writeFds))
				{
                    // printf("req done = %d\n", it->request.ReqstDone);
                    printf("req done = %s\n", it->request.getErrorMsg().c_str());
					if (!it->response.Config)
						it->response.startResponse(*it);
					it->response.checkErrorsRequest(*it);
                    if(!it->response.responseDone)
                    {
					    if (it->response.isCgi)
					    	it->response.CGI(*it);
					    else
					    	it->response.handleNormalFiles(*it);
                    }
					it->response.sendResponse(*it);
					if (it->response.responseSent)
					{
						if (it->response.closeClient)
						{
							logMessage(SCLOSE, it->clientHost, it->socketClient, "Response: Close conection from " + it->clientIP);
							close(it->socketClient);
							it->response.clear();
							it->request.reset();
							clients.erase(it);
							continue;
						}
						// std::cout << it->response.fptr << std::endl;
						// fclose(it->response.fptr);
						it->response.clear();
						it->request.reset();
						it->readEvent = true;
						it->writeEvent = false;
					}
				}
				it++;
			}
		}
		acceptClients();
	}
	delete[] recvbuffer;
}

bool RunServers::receiveData(vector<Client>::iterator &it)
{
    ssize_t size = recv(it->socketClient, recvbuffer, 4096 * 4, 0);
	if (size <= 0)
	{
		logMessage(SCLOSE, it->clientHost, it->socketClient, "Request: Close conection from " + it->clientIP);
		close(it->socketClient);
		it->request.reset();
		clients.erase(it);
        return false;
	}
	string buffer(recvbuffer, size);
    // std::ofstream outf("/Users/yettabaa/Desktop/webservemerge/www/DebugFile.txt", std::ios::out | std::ios::app | std::ios::binary);
    //                     outf << buffer;
    size_t timeMilSec;
    it->request.setTimeLastData((timeofday(timeMilSec)) ? timeMilSec : 0);
	it->request.read(*it, buffer, size);
	if (it->request.ReqstDone)
	{
		std::string method_ = "[" FG_YELLOW + it->request.Method + RESET_ALL + "]";
		std::string URI = "[" FG_YELLOW + it->request.URI + RESET_ALL + "]";
		logMessage(SREQ, it->clientHost, it->socketClient, method_ + " Received Data from " + it->clientIP);
		logMessage(SINFO, it->clientHost, it->socketClient," URI : " + URI);
		it->readEvent = false;
		it->writeEvent = true;
	}
	buffer.clear();
    return true;
}

void RunServers::resetFds()
{
	FD_ZERO(&readFds);
	FD_ZERO(&writeFds);
	readFds = serverFds;
	maxFds = maxFdstmp;
	for (vector<Client>::iterator it = clients.begin(); it != clients.end();
		 it++)
	{
		if (it->readEvent)
			FD_SET(it->socketClient, &readFds);
		else if (it->writeEvent)
			FD_SET(it->socketClient, &writeFds);
		if (it->socketClient > maxFds)
			maxFds = it->socketClient;
	}
}

void RunServers::acceptClients()
{
	for (vector<Server>::iterator it = servers.begin(); it != servers.end(); it++)
	{
		if (FD_ISSET(it->socketServer, &readFds))
		{
			newSocket = -1;
			struct sockaddr_in clientAddr;
			socklen_t clientAddrLen = sizeof(clientAddr);
			memset(&clientAddr, 0, sizeof(clientAddr));
			std::string host = "http://" + it->serverConf.DefaultServerConfig.Host + ":" + it->serverConf.DefaultServerConfig.Port;
			newSocket = accept(it->socketServer, (struct sockaddr *)&clientAddr, &clientAddrLen);
			if (newSocket == -1)
			{
				logMessage(SERROR, host, newSocket, "Accept failed");
			}
			else
			{
				std::string clientIP = inet_ntoa(clientAddr.sin_addr);
				logMessage(SACCEPT, host, newSocket, "Accept connection from " + clientIP);
				Client client(it->serverConf, newSocket, clientIP, host);
				clients.push_back(client);
			}
		}
	}
}

void RunServers::timeoutClientChecker(Client &client)
{
    size_t timeMilSec;

    if (client.readEvent == true && client.request.getTimeLastData() != 0 && timeofday(timeMilSec)
    && (timeMilSec - client.request.getTimeLastData()) >= CLIENT_BODY_TIMEOUT)
    {
        // printf("client_body_timeout = %lld\n",(timeofday() - client.request.timeLastData ));
        client.request.ReqstDone = 408;
        client.readEvent = false;
        client.writeEvent = true;
    }
}

void RunServers::timeoutChecker()
{
    std::cout << "Timeout\n"; //!!
    size_t timeMilSec;

    for (vector<Client>::iterator it = clients.begin(); it != clients.end(); it++)
    {
        // printf("client_body_timeout = %lld fd ==> %d\n",(timeofday() - it->request.timeLastData), it->socketClient);
        if (it->readEvent == true && it->request.getTimeLastData() != 0 && timeofday(timeMilSec)
        && (timeMilSec - it->request.getTimeLastData()) >= CLIENT_BODY_TIMEOUT)
        {
            // printf("408\n");
            it->request.ReqstDone = 408;
            it->readEvent = false;
            it->writeEvent = true;
        }
    }
}

int RunServers::bindSockets(Server &server)
{
	// Create a socket
	if ((server.socketServer = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		throw std::runtime_error("Error: Failed to create socket");
	// Enable socket address reuse
	int enableReuse = 1;
	if (setsockopt(server.socketServer, SOL_SOCKET, SO_REUSEADDR, &enableReuse,
				   sizeof(enableReuse)) < 0)
		throw std::runtime_error("Error: Failed to setsockopt for reuse");
	// Define and configure the server address
	struct sockaddr_in serverAddr;
	std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET; // Set the address family to AF_INET (for IPv4)
	serverAddr.sin_port = htons(std::atoi(server.serverConf.DefaultServerConfig.Port.c_str()));
	inet_pton(AF_INET, server.serverConf.DefaultServerConfig.Host.c_str(),
			  &serverAddr.sin_addr);
	// Bind the socket to the server address
	if (::bind(server.socketServer, (struct sockaddr *)&serverAddr,
			   sizeof(serverAddr)) == -1)
		throw std::runtime_error("Error: Failed to bind socket to address");
	// Listen for incoming connections
	if (listen(server.socketServer, FD_SETSIZE) < 0)
		throw std::runtime_error("Error: Failed to listen on socket");
	// Set the socket to non-blocking mode
	if (fcntl(server.socketServer, F_SETFL, O_NONBLOCK , FD_CLOEXEC) < 0)
		throw std::runtime_error("Error: Failed to set socket to non-blocking mode");
	// Disable SIGPIPE to prevent the program from being terminated when writing
	// to a closed socket.
    signal(SIGPIPE, SIG_IGN);
	// int noSigpipe = 1;
	// if (setsockopt(server.socketServer, SOL_SOCKET, SO_NOSIGPIPE, &noSigpipe, sizeof(noSigpipe)) < 0)
	// 	throw std::runtime_error("Error: Failed to disable SIGPIPE");
	// Print the socket descriptor for debugging purposes
	std::string host = "http://" + server.serverConf.DefaultServerConfig.Host + ":" + server.serverConf.DefaultServerConfig.Port;
	logMessage(SINFO, host, server.socketServer, "Server start listening");
	return server.socketServer;
}

RunServers::RunServers(char **av) : numberOfEvents(0)
{
	Config config;
	config.parser(av[1]);
	config.checkServerConfig(av[1]);
	config.filterServerConfig();
#ifdef DEBUG_C
	config.printServers();
#endif
	vector<ServerConf> &serverConf = config.getServerConfig();
    timeout.tv_sec = CLIENT_BODY_TIMEOUT / 1000;
    timeout.tv_usec = 0;
	maxFdstmp = -1;
    printf("size client = %ld\n", sizeof(Client));
	for (vector<ServerConf>::iterator it = serverConf.begin();
		 it != serverConf.end(); it++)
	{
		Server serv;
		serv.serverConf = *it;
		int fd = bindSockets(serv);
		if (fd > maxFdstmp)
			maxFdstmp = fd;
		servers.push_back(serv);
	}
	FD_ZERO(&serverFds);
	for (vector<Server>::iterator it = servers.begin(); it != servers.end(); it++)
		FD_SET(it->socketServer, &serverFds);
}

RunServers::~RunServers() {}
