#include "../../include//Server/RunServers.hpp"

#define forever for (;;)

void RunServers::runing()
{
    char *bufRecv = new char[4096 * 4]; //delete it

    FD_ZERO(&serverFds);
    for (vector<Server>::iterator it = servers.begin(); it != servers.end(); it++)
        FD_SET(it->socketServer, &serverFds);
    forever {
        resetFds();
        numberOfEvents = select(maxFds +1, &readFds, &writeFds, NULL, NULL);
        if (numberOfEvents <= -1) {
            std::cout << "Error in select function\n"; ///!!
        } else if (numberOfEvents == 0) {
            std::cout << "Timeout\n"; //!!
        } else {
            for (vector<Client>::iterator it = clients.begin(); it != clients.end(); ) {
                if (FD_ISSET(it->socketClient, &readFds)) {
                    ssize_t size = recv(it->socketClient, bufRecv, 4096 * 4, 0);
                    if (size <= -1) {

                    } else if (size == 0) {
                        close(it->socketClient);
                        clients.erase(it);
                        continue;
                    } else {
                        string buffer(bufRecv, size);
                        it->request.read(buffer, size);
                        //    std::cout << bufRecv << std::endl;
                        // printf("%d\n", it->request.ReqstDone);
                        if (it->request.ReqstDone) {
                            printf("data complet from  = %d\n", it->socketClient);///!!!
                            it->read = false;
                            it->write = true;
                        }
                        buffer.clear();
                    }
                } else if (FD_ISSET(it->socketClient, &writeFds)) {
                    printf("data send to  = %d\n", it->socketClient);
                    string body = _Response(*it);
                    // cout << body << std::endl;
                    if (send(it->socketClient, &body[0], body.length(), 0) <= 0)
                        std::cout << "Error sending data\n";
                    it->read = true;
                    it->write = false;
                    it->request.clear();
                }
                it++;
            }
        }
        acceptClients();
    }
}

void RunServers::resetFds()
{
    FD_ZERO(&readFds);
    FD_ZERO(&writeFds);
    readFds = serverFds;
    for (vector<Client>::iterator it = clients.begin(); it != clients.end(); it++) {
        if (it->read)
            FD_SET(it->socketClient, &readFds);
        else if (it->write)
            FD_SET(it->socketClient, &writeFds);
    }
}

void RunServers::acceptClients()
{
    for (vector<Server>::iterator it = servers.begin(); it != servers.end(); it++) {
        if (FD_ISSET(it->socketServer, &readFds)) {
            newSocket = accept(it->socketServer, NULL, NULL); //way we pute NULL ??
            if (newSocket <= -1) {
                /// !!
            } else {
                printf("new client fd = %d\n", newSocket); ///!!
                Client client(newSocket);
                clients.push_back(client);
                if (newSocket > maxFds)
                    maxFds = newSocket;
            }
        }
    }
}

int RunServers::bindSockets(Server& server)
{
    	// Create a socket
	if ((server.socketServer = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		throw std::runtime_error("Error: Failed to create socket");
	// Enable socket address reuse
	int enableReuse = 1;
	if (setsockopt(server.socketServer, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(enableReuse)) < 0)
		throw std::runtime_error("Error: Failed to setsockopt for reuse");
	// Define and configure the server address
	struct sockaddr_in serverAddr;
	std::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET; // Set the address family to AF_INET (for IPv4)
	serverAddr.sin_port = htons(std::atoi(server.serverConf.DefaultServerConfig.Port.c_str()));
	inet_pton(AF_INET, server.serverConf.DefaultServerConfig.Host.c_str(), &serverAddr.sin_addr);
	// Bind the socket to the server address
	if (bind(server.socketServer, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
		throw std::runtime_error("Error: Failed to bind socket to address");
	// Listen for incoming connections
	if (listen(server.socketServer, FD_SETSIZE) < 0)
		throw std::runtime_error("Error: Failed to listen on socket");
	// Set the socket to non-blocking mode
	if (fcntl(server.socketServer, F_SETFL, O_NONBLOCK | FD_CLOEXEC) < 0)
		throw std::runtime_error("Error: Failed to set socket to non-blocking mode");
	// Disable SIGPIPE to prevent the program from being terminated when writing to a closed socket.
	int noSigpipe = 1;
	if (setsockopt(server.socketServer, SOL_SOCKET, SO_NOSIGPIPE, &noSigpipe, sizeof(noSigpipe)) < 0)
		throw std::runtime_error("Error: Failed to disable SIGPIPE");
	// Print the socket descriptor for debugging purposes
	logMessage(SINFO, "Server listening on http://" + server.serverConf.DefaultServerConfig.Host + ":" + server.serverConf.DefaultServerConfig.Port);
	std::cout << BOLD FG_BLUE "Server socket created with descriptor: " << server.socketServer << RESET_ALL << std::endl;
    return server.socketServer;
}

RunServers::RunServers(vector<ServerConf>& serverConf) : numberOfEvents(0)
{
    maxFds = -1;
    for (vector<ServerConf>::iterator it = serverConf.begin(); it != serverConf.end(); it++) {
			Server serv;
			serv.serverConf = *it;
			int fd = bindSockets(serv);
            if (fd > maxFds)
                maxFds = fd;
			servers.push_back(serv);
	}
}

RunServers::~RunServers() {}
