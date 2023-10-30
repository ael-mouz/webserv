#include "../Include/Server.hpp"

void Server::init()
{
	// Create a socket
	if ((this->socketServer = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		throw std::runtime_error("Error: Failed to create socket");
	// Set the maximum file descriptor for select()
	maxfd = this->socketServer;
	// Enable socket address reuse
	int enableReuse = 1;
	if (setsockopt(this->socketServer, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(enableReuse)) < 0)
		throw std::runtime_error("Error: Failed to setsockopt for reuse");
	// Define and configure the server address
	struct sockaddr_in serverAddr;
	std::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET; // Set the address family to AF_INET (for IPv4)
	serverAddr.sin_port = htons(std::atoi(this->serverConf.DefaultServerConfig.Port.c_str()));
	inet_pton(AF_INET, this->serverConf.DefaultServerConfig.Host.c_str(), &serverAddr.sin_addr);
	// Bind the socket to the server address
	if (bind(this->socketServer, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
		throw std::runtime_error("Error: Failed to bind socket to address");
	// Listen for incoming connections
	if (listen(this->socketServer, FD_SETSIZE) < 0)
		throw std::runtime_error("Error: Failed to listen on socket");
	// Set the socket to non-blocking mode
	if (fcntl(this->socketServer, F_SETFL, O_NONBLOCK | FD_CLOEXEC) < 0)
		throw std::runtime_error("Error: Failed to set socket to non-blocking mode");
	// Disable SIGPIPE to prevent the program from being terminated when writing to a closed socket.
	int noSigpipe = 1;
	if (setsockopt(this->socketServer, SOL_SOCKET, SO_NOSIGPIPE, &noSigpipe, sizeof(noSigpipe)) < 0)
		throw std::runtime_error("Error: Failed to disable SIGPIPE");
	// Print the socket descriptor for debugging purposes
	logMessage(SINFO, "Server listening on http://" + this->serverConf.DefaultServerConfig.Host + ":" + this->serverConf.DefaultServerConfig.Port);
	std::cout << BOLD FG_BLUE "Server socket created with descriptor: " << this->socketServer << RESET_ALL<< std::endl;
}

// Print the values of addr for debugging
// std::cout << "Server Address Configuration:" << std::endl;
// std::cout << "sin_family: " << serverAddr.sin_family << std::endl;
// char ip[INET_ADDRSTRLEN];
// inet_ntop(AF_INET, &serverAddr.sin_addr, ip, INET_ADDRSTRLEN);
// std::cout << "sin_addr: " << ip << std::endl;
// std::cout << "sin_port: " << ntohs(serverAddr.sin_port) << std::endl;
