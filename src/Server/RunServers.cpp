#include "RunServers.hpp"
// #include "../../include/Server/Utils.hpp"
// #include "../../include/Server/Server.hpp"
// #include "../../include/Server/Client.hpp"
// #include "../../include/Config/ServerConf.hpp"
#define forever for (;;)

void RunServers::init(vector<ServerConf>& ServerConf)
{
    for (size_t i  = 0; i < ServerConf.size(); i++) {
			Server serv;
			serv.serverConf = ServerConf[i];
			int fd = serv.init();
            if (fd > maxFds)
                maxFds = fd;
			servers.push_back(serv);
	}
}

// void RunServers::runingServers(vector<Server>& servers, int maxfd)
void RunServers::runingServers()
{
    // vector<Client> clients;
    // fd_set  tmp, readFds, writeFds;
    // struct timeval timeout;
    char *bufRecv = new char[4096 * 4];
    int numberOfEvents, newSocket;
    FD_ZERO(&tmp);
    for (vector<Server>::iterator it = servers.begin(); it != servers.end(); it++)
        FD_SET(it->socketServer, &tmp);
    forever {
        FD_ZERO(&readFds);
        FD_ZERO(&writeFds);
        readFds = tmp;
        for (vector<Client>::iterator it = clients.begin(); it != clients.end(); it++) {
            if (it->read)
                FD_SET(it->socketClient, &readFds);
            else if (it->write)
                FD_SET(it->socketClient, &writeFds);
        }
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
        for (vector<Server>::iterator it = servers.begin(); it != servers.end(); it++) {
            if (FD_ISSET(it->socketServer, &readFds)) {
                newSocket = accept(it->socketServer, NULL, NULL); //way we pute NULL ??
                if (newSocket <= -1) {

                } else {
                    printf("new client fd = %d\n", newSocket); ///!!
                    Client client(it->serverConf, newSocket);
                    clients.push_back(client);
                    if (newSocket > maxFds)
                        maxFds = newSocket;
                }
            }
        }
    }
}