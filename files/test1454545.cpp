#include <iostream>
#include <cstring>
#include <cstdlib>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
int main()
{
    // Create an IPv4 socket bound to 127.0.0.1
    int ipv4Socket = socket(AF_INET, SOCK_STREAM, 0);
    if (ipv4Socket == -1)
    {
        std::cerr << "IPv4 socket creation failed." << std::endl;
        return 1;
    }

    struct sockaddr_in ipv4Address;
    std::memset(&ipv4Address, 0, sizeof(ipv4Address));
    ipv4Address.sin_family = AF_INET;
    ipv4Address.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &ipv4Address.sin_addr);

    if (bind(ipv4Socket, (struct sockaddr *)&ipv4Address, sizeof(ipv4Address)) == -1)
    {
        std::cerr << "IPv4 socket binding failed." << std::endl;
        close(ipv4Socket);
        return 1;
    }

    // Create an IPv6 socket bound to ::1
    int ipv6Socket = socket(AF_INET6, SOCK_STREAM, 0);
    if (ipv6Socket == -1)
    {
        std::cerr << "IPv6 socket creation failed." << std::endl;
        return 1;
    }

    struct sockaddr_in6 ipv6Address;
    std::memset(&ipv6Address, 0, sizeof(ipv6Address));
    ipv6Address.sin6_family = AF_INET6;
    ipv6Address.sin6_port = htons(8080);
    inet_pton(AF_INET6, "127.0.0.1", &ipv6Address.sin6_addr);

    if (bind(ipv6Socket, (struct sockaddr *)&ipv6Address, sizeof(ipv6Address)) == -1)
    {
        std::cerr << "IPv6 socket binding failed." << std::endl;
        close(ipv6Socket);
        return 1;
    }

    // Rest of your code...

    return 0;
}
