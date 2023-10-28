#include <iostream>
#include <cstring>
#include <cstdlib>
#include <netdb.h>
#include <arpa/inet.h>

int main()
{
    struct addrinfo hints, *res;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; // Allow both IPv4 and IPv6

    int status = getaddrinfo("youtube.com", "1511", &hints, &res);
    if (status != 0)
    {
        std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
        return 1;
    }

    struct addrinfo *p;
    char ipstr[INET_ADDRSTRLEN]; // Sufficient for IPv4 addresses

    for (p = res; p != NULL; p = p->ai_next)
    {
        if (p->ai_family == AF_INET)
        { // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            std::string ip_version = "IPv4";

            // Convert the IPv4 address to a human-readable string
            std::string ip_address = inet_ntoa(ipv4->sin_addr);
            int port = ntohs(ipv4->sin_port);

            std::cout << "IP Version: " << ip_version << std::endl;
            std::cout << "IP Address: " << ip_address << std::endl;
            std::cout << "Port: " << port << std::endl;
            std::cout << "Socket Type: " << p->ai_socktype << std::endl;
            std::cout << "Protocol: " << p->ai_protocol << std::endl
                      << std::endl;
        }
    }

    freeaddrinfo(res); // Don't forget to free the memory
    return 0;
}
