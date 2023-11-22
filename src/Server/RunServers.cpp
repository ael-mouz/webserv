#include "../../include//Server/RunServers.hpp"

#define forever for (;;)

void RunServers::runing()
{
	recvbuffer = new char[4096 * 4];
	forever
	{
		resetFds();
		// numberOfEvents = select(maxFds + 1, &readFds, &writeFds, NULL, &timeout);
        // printf("fd = %ld\n",maxFdstmp+clients.size());
        numberOfEvents = poll(&fds[0], fds.size(), 3000);
		if (numberOfEvents <  0)
        {
			std::cout << "Error in select function\n"; ///!!
            hardReset();
            continue;
        }
		else if (numberOfEvents == 0)
        {

			std::cout << "timout\n"; ///!!
			timeoutChecker();
        }
		else
		{
			for (vector<Client>::iterator it = clients.begin(); it != clients.end();)
			{
				timeoutClientChecker(*it);
				// if (FD_ISSET(it->socketClient, &readFds))
				if (fds[maxFdstmp + it - clients.begin()].revents & POLLIN)
				{
					if (receiveData(it) == false)
						continue;

				}
				else if (fds[maxFdstmp + it - clients.begin()].revents & POLLOUT)
				{
					if (sendData(it) == false)
						continue;
				}
				else if (fds[maxFdstmp + it - clients.begin()].revents & POLLHUP)
				{
                    logMessage(SCLOSE, it->clientHost, it->socketClient, "Response: Close conection from " + it->clientIP);
                    shutdown(it->socketClient, SHUT_RDWR);
			        close(it->socketClient);
			        it->response.clear();
			        it->request.reset();
                    fds.erase(fds.begin() + (maxFdstmp + it - clients.begin()));
			        clients.erase(it);
                    continue;
					// if (sendData(it) == false)
					// 	continue;
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
        shutdown(it->socketClient, SHUT_RDWR);
		close(it->socketClient);
		it->request.reset();
        fds.erase(fds.begin() + (maxFdstmp + it - clients.begin()));
		clients.erase(it);
		return false;
	}
    // printf("size = %ld\n", size);
	string buffer(recvbuffer, size);
	// std::ofstream outf("/Users/yettabaa/Desktop/webservemerge/www/DebugFile.txt", std::ios::out | std::ios::app | std::ios::binary);
	// outf << "-----------------------------------\n";
	// outf << buffer;
	size_t timeMilSec;
	it->request.setTimeLastData((timeofday(timeMilSec)) ? timeMilSec : 0);
	it->request.read(*it, buffer, size);
	if (it->request.ReqstDone)
	{
        // printf("req Done = %d\n", it->request.ReqstDone);
		std::string method_ = "[" FG_YELLOW + it->request.Method + RESET_ALL + "]";
		std::string URI = "[" FG_YELLOW + it->request.URI + RESET_ALL + "]";
		if (!it->request.Method.empty())
			logMessage(SREQ, it->clientHost, it->socketClient, method_ + " Received Data from " + it->clientIP);
		if (!it->request.URI.empty())
			logMessage(SINFO, it->clientHost, it->socketClient, " URI : " + URI);
		it->readEvent = false;
		it->writeEvent = true;
	}
	buffer.clear();
	return true;
}

bool RunServers::sendData(vector<Client>::iterator &it)
{
	if (!it->response.Config)
		it->response.startResponse(*it);
	it->response.checkErrorsRequest(*it);
	if (!it->response.responseDone)
	{
		if (it->response.isCgi)
			it->response.CGI(*it);
		else
			it->response.handleNormalFiles(*it);
	}
	it->response.sendResponse(*it);
	if (it->response.responseSent)
	{
		if (!it->request.getErrorMsg().empty())
			logMessage(SDEBUG, it->clientHost, it->socketClient, it->request.getErrorMsg());
		if (it->response.closeClient)
		{
			logMessage(SCLOSE, it->clientHost, it->socketClient, "Response: Close conection from " + it->clientIP);
            shutdown(it->socketClient, SHUT_RDWR);
			close(it->socketClient);
			it->response.clear();
			it->request.reset();
            fds.erase(fds.begin() + (maxFdstmp + it - clients.begin()));
			clients.erase(it);
			return false;
		}
		it->response.clear();
		it->request.reset();
		it->readEvent = true;
		it->writeEvent = false;
	}
	return true;
}

void RunServers::resetFds()
{
    // puts("--------------------");
	for (vector<Client>::iterator it = clients.begin(); it != clients.end(); it++)
	{
		if (it->readEvent)
            fds[maxFdstmp + it - clients.begin()].events = POLLIN;
		else if (it->writeEvent)
            fds[maxFdstmp + it - clients.begin()].events = POLLOUT | POLLHUP;
        // fds.push_back(fds1);
        //  printf("ind = %ld fd = %d\n",maxFdstmp + (it - clients.begin()), fds[maxFdstmp + (it - clients.begin())].fd);
	}
}

void RunServers::acceptClients()
{
	for (vector<Server>::iterator it = servers.begin(); it != servers.end(); it++)
	{
		// if (FD_ISSET(it->socketServer, &readFds))
        //  printf("ind = %ld fd = %d\n",it - servers.begin(), fds[it-servers.begin()].fd);
		if (fds[it - servers.begin()].revents & POLLIN)
		{
			newSocket = -1;
			struct sockaddr_in clientAddr;
			socklen_t clientAddrLen = sizeof(clientAddr);
			memset(&clientAddr, 0, sizeof(clientAddr));
			std::string host = "http://" + it->serverConf.DefaultServerConfig.Host + ":" + it->serverConf.DefaultServerConfig.Port;
			newSocket = accept(it->socketServer, (struct sockaddr *)&clientAddr, &clientAddrLen);
			if (newSocket == -1)
				logMessage(SERROR, host, newSocket, "Accept failed");
			else
			{
				std::string clientIP = inet_ntoa(clientAddr.sin_addr);
				logMessage(SACCEPT, host, newSocket, "Accept connection from " + clientIP);
                fcntl(newSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
				Client client(it->serverConf, newSocket, clientIP, host);
				clients.push_back(client);
                struct pollfd fds1;

                fds1.fd = newSocket;
                fds1.revents = 0;
                fds1.events = POLLIN;
                fds.push_back(fds1);
			}
		}
	}
}

void RunServers::hardReset()
{
    FD_ZERO(&readFds);
	FD_ZERO(&writeFds);
	readFds = serverFds;
	maxFds = maxFdstmp;
	for (vector<Client>::iterator it = clients.begin(); it != clients.end(); )
	{
		shutdown(it->socketClient, SHUT_RDWR);
		close(it->socketClient);
        it->response.clear();
		it->request.reset();
		clients.erase(it);
	}
    clients.clear();
}

void RunServers::timeoutClientChecker(Client &client)
{
	size_t timeMilSec;

	if (client.readEvent == true && client.request.getTimeLastData() != 0 && timeofday(timeMilSec) && (timeMilSec - client.request.getTimeLastData()) >= CLIENT_BODY_TIMEOUT)
	{
		client.request.ReqstDone = 408;
		client.readEvent = false;
		client.writeEvent = true;
	}
}

void RunServers::timeoutChecker()
{
	size_t timeMilSec;

	for (vector<Client>::iterator it = clients.begin(); it != clients.end(); it++)
	{
		if (it->readEvent == true && it->request.getTimeLastData() != 0 && timeofday(timeMilSec) && (timeMilSec - it->request.getTimeLastData()) >= CLIENT_BODY_TIMEOUT)
		{
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
	if (setsockopt(server.socketServer, SOL_SOCKET, SO_REUSEPORT, &enableReuse,
				   sizeof(enableReuse)) < 0)
		throw std::runtime_error("Error: Failed to setsockopt for reuse");
	enableReuse = 1;
	if (setsockopt(server.socketServer, SOL_SOCKET, SO_REUSEADDR, &enableReuse,
				   sizeof(enableReuse)) < 0)
		throw std::runtime_error("Error: Failed to setsockopt for reuse");
	// Set the socket to non-blocking mode
	if (fcntl(server.socketServer, F_SETFL, O_NONBLOCK, FD_CLOEXEC) < 0)
		throw std::runtime_error("Error: Failed to set socket to non-blocking mode");
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
	if (listen(server.socketServer, 128) < 0)
		throw std::runtime_error("Error: Failed to listen on socket");
	// Disable SIGPIPE to prevent the program from being terminated when writing to a closed socket.
	signal(SIGPIPE, SIG_IGN);
	std::string host = "http://" + server.serverConf.DefaultServerConfig.Host + ":" + server.serverConf.DefaultServerConfig.Port;
	logMessage(SINFO, host, server.socketServer, "Server start listening");
	return server.socketServer;
}

RunServers::RunServers(char **av) : numberOfEvents(0)
{
	Config config;
	config.parser(av[1]);
#ifdef DEBUG_C
	config.printServers();
#endif
	vector<ServerConf> &serverConf = config.getServerConfig();
	// timeout.tv_sec = CLIENT_BODY_TIMEOUT / 1000;
	// timeout.tv_usec = 0;
    timeout = CLIENT_BODY_TIMEOUT;
	maxFdstmp = 0;
	for (vector<ServerConf>::iterator it = serverConf.begin();
		 it != serverConf.end(); it++)
	{
		Server serv;
		serv.serverConf = *it;
		int fd = bindSockets(serv);
		if (fd > maxFdstmp)
			maxFdstmp++;
		servers.push_back(serv);
	}
	// FD_ZERO(&serverFds);
    // printf("%d\n", maxFdstmp);
	for (vector<Server>::iterator it = servers.begin(); it != servers.end(); it++)
    {
        struct pollfd fds1;
        fds1.fd = it->socketServer;
        fds1.events = POLLIN;
        fds1.revents = 0;
        fds.push_back(fds1);
        // printf("ind = %ld fd = %d\n",it - servers.begin(), it->socketServer);

    }
	// FD_ZERO(&serverFds);
	// for (vector<Server>::iterator it = servers.begin(); it != servers.end(); it++)
	// 	FD_SET(it->socketServer, &serverFds);
}

RunServers::~RunServers() {}
