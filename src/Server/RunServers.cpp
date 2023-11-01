#include "../../include/Server/Utils.hpp"
#include "../../include/Server/Server.hpp"
#include "../../include/Server/Client.hpp"

void RunServers(vector<Server> &servers, int maxfd)
{
	vector<Client> Clients;
	fd_set ReadFds, WriteFds;
	// struct timeval timeout;
	char *bufRecv = new char[4096 * 4];
	int NumberOfEvents, newSocket;

	while (true)
	{
		FD_ZERO(&ReadFds);
		FD_ZERO(&WriteFds);
		for (vector<Server>::iterator it = servers.begin(); it != servers.end(); it++)
			FD_SET(it->socketServer, &ReadFds);
		for (vector<Client>::iterator it = Clients.begin(); it != Clients.end(); it++)
		{
			if (it->read)
				FD_SET(it->socketClient, &ReadFds);
			else if (it->write)
				FD_SET(it->socketClient, &WriteFds);
		}
		NumberOfEvents = select(maxfd + 1, &ReadFds, &WriteFds, NULL, NULL);
		if (NumberOfEvents <= -1)
		{
			std::cout << "Error in select function\n"; ///!!
		}
		else if (NumberOfEvents == 0)
		{
			std::cout << "Timeout\n"; //!!
		}
		else
		{
			for (vector<Client>::iterator it = Clients.begin(); it != Clients.end();)
			{
				if (FD_ISSET(it->socketClient, &ReadFds))
				{
					ssize_t size = recv(it->socketClient, bufRecv, 4096 * 4, 0);
					std::cout << "data come from ::: " << it->socketClient << std::endl;
					if (size <= -1)
						std::cout << "recv error : " << it->socketClient << std::endl;
					else if (size == 0)
					{
						close(it->socketClient);
						std::cout << "closed : " << it->socketClient << std::endl;
						Clients.erase(it);
						continue;
					}
					else
					{
						string buffer(bufRecv, size);
						it->request.read(buffer, size);
						//    std::cout << bufRecv << std::endl;
						if (it->request.ReqstDone)
						{
							printf("data complet from  = %d\n", it->socketClient); ///!!!
							it->read = false;
							it->write = true;
						}
					}
				}
				else if (FD_ISSET(it->socketClient, &WriteFds))
				{
					// printf("data send to  = %d\n", it->socketClient);
					string body = _Response(*it);
					it->response.response(*it);
					it->response.sendResponse(*it);
					// cout << body << std::endl;
					// if (send(it->socketClient, &body[0], body.length(), 0) <= 0)
					// 	std::cout << "Error sending data\n";
					if (it->response.responseSent)
					{
						// close(it->socketClient);
						it->read = true;
						it->write = false;
						it->request.clear();
					}
				}
				it++;
			}
		}
		for (vector<Server>::iterator it = servers.begin(); it != servers.end(); it++)
		{
			if (FD_ISSET(it->socketServer, &ReadFds))
			{
				newSocket = accept(it->socketServer, NULL, NULL); // way we pute NULL ??
				if (newSocket <= -1)
				{
				}
				else
				{
					// printf("new client fd = %d\n", newSocket); ///!!
					std::cout << "| server : " << it->socketServer << "| client: " << newSocket << "|" << std::endl;
					Client client(it->serverConf, newSocket);
					Clients.push_back(client);
					if (newSocket > maxfd)
						maxfd = newSocket;
				}
			}
		}
	}
}
