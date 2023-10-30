// #include "../../include/Utils.hpp"
// #include "../../include/Client/Client.hpp"

// int main(int ac, char *av[])
// {
//     (void)ac;
//     (void)av;
//     try
//     {
//         fd_set ReadFds, WriteFds;//, ErrorFds;
//         sockaddr_in addr;
//         struct timeval timeout;
//         // socklen_t addrlen = sizeof(addr);
//         int listening, readsocks, new_sock, maxfd;
//         std::vector<Client> Clients;
//         // std::vector<int> track_fds;

//         listening = socket(AF_INET, SOCK_STREAM, 0); //AF_INET == IPv4 Internet protocols, SOCK_STREAM == TCP
//         if (listening == -1)
//             throw std::runtime_error("Error: Failed to create socket");
//         maxfd = listening;
//         printf("sockfd = %d\n", listening); //!!
//         int reuse = 1;
//         if (setsockopt(listening, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) //reuse the same port without probleme
//             throw std::runtime_error("Error: Failed to setsockopt for reuse");

//         std::memset(&addr, 0, sizeof(addr));
//         addr.sin_family = AF_INET;
//         addr.sin_addr.s_addr = htonl(INADDR_ANY);// Listen to port 8080 on any address
//         addr.sin_port = htons(8000);
//         if (bind(listening, (struct sockaddr *)&addr, sizeof(addr)) < 0) //assign an IP address and port to the socket.
//             throw std::runtime_error("Error: Failed to bind socket");
//         if (listen(listening, FD_SETSIZE) < 0) //marks a socket as passive. (The socket will be used to accept connections)
//             throw std::runtime_error("Error: Failed to listen on socket");
//         if (fcntl(listening, F_SETFL, O_NONBLOCK, FD_CLOEXEC) < 0) // protect????
//             throw std::runtime_error("Error: Failed to set socket to nonblocking mode");

//         char *buf = new char[4096 * 4]; //delete (leaks) and protect

//         while (true)
//         {
//             // puts("ss");
//             timeout.tv_sec = 2;
//             timeout.tv_usec = 0;
//             signal(SIGPIPE, SIG_IGN);
//             FD_ZERO(&ReadFds);
//             FD_ZERO(&WriteFds);
//             FD_SET(listening, &ReadFds);

//             for (size_t i = 0; i < Clients.size(); i++)
//             {
//                 // printf("data recive from  = %d\n", Clients[i].fd);
//                 if (Clients[i].read)
//                     FD_SET(Clients[i].fd, &ReadFds);
//                 if (Clients[i].write)
//                     FD_SET(Clients[i].fd, &WriteFds);
//             }
//             readsocks = select(maxfd + 1, &ReadFds, &WriteFds, NULL, NULL);
//             if (readsocks == -1)
//             {
//                 std::cout << "Error in select function\n";
//                 // continue;
//             }
//             else if (!readsocks)
//             {
//                 std::cout << "Timeout\n";
//                 continue;
//             }
//             else
//             {
//                 for (size_t i = 0; i < Clients.size(); i++)
//                 {
//                     // int close_fd =false;
//                     if (FD_ISSET(Clients[i].fd, &ReadFds))
//                     {
//                         // printf("data recive from  = %d\n", Clients[i].fd);
//                         ssize_t size = recv(Clients[i].fd, buf, 4096 * 4, 0);
//                         if (!size)
//                         {
//                             close(Clients[i].fd);
//                             Clients.erase(Clients.begin()+ i);
//                             continue;
//                             // if (!flag) {
//                             //     it = vector2.erase(it);
//                             // } else {
//                             //     ++it;
//                             // }
//                         }
//                         // std::cout << "fd = "<< Clients[i].fd <<" recv size= " << size << "\n";
//                         if (size <= 0)
//                         {
//                             continue;
//                         }
//                         std::string buffer(buf, size);
//                         // outf << "\n----------------Request-----------------\n\n";
//                         Clients[i].request.read(buffer, size);
//                         // std::ofstream outf("/goinfre/yettabaa/mqawed.txt", std::ios::out | std::ios::app | std::ios::binary);
//                         // outf << buffer;
//                         // std::cout << buffer <<"\n";

//                         if (Clients[i].request.ReqstDone)
//                         {
//                             printf("data complet from  = %d\n", Clients[i].fd);
//                             Clients[i].read = false;
//                             Clients[i].write = true;
//                         }
//                     }
//                     if (FD_ISSET(Clients[i].fd, &WriteFds))
//                     {
//                         printf("data send to  = %d\n", Clients[i].fd);
//                         std::string body = Response(Clients[i]);
//                         if (send(Clients[i].fd, body.c_str(), body.length(), 0) <= 0)
//                             std::cout << "Error sending data\n";
//                         // outf << "\n----------------Response-----------------\n\n";
//                         // outf << body;
//                         Clients[i].read = true;
//                         Clients[i].write = false;
//                         Clients[i].request.clear();
//                     }
//                     // if (close_fd) {
//                     //     // printf("Delete fd = %d\n", *it);
//                     //     close(*it);
//                     //     it = track_fds.erase(it);
//                     // }
//                     // else
//                     //     ++it;
//                 }
//                 if (FD_ISSET(listening, &ReadFds))
//                 {
//                     if ((new_sock = accept(listening, NULL, NULL)) < 0)
//                         std::cout << "Error in accepting new connection\n";
//                     else
//                     {
//                         printf("new client fd = %d\n", new_sock);
//                         Clients.push_back(new_sock);
//                         if (new_sock > maxfd)
//                             maxfd = new_sock;
//                     }
//                 }
//             }
//         }

//     }
//     catch(const std::exception& e)
//     {
//         std::cerr << e.what() << '\n';
//     }

//     return 0;
// }
