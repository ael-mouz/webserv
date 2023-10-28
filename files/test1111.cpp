#include <iostream>
#include <cstring>
#include <cstdlib>
#include <netdb.h>
#include <arpa/inet.h>

std::string getIpv4Address(const std::string &hostname)
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    std::string ipAddress;

    std::memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    int status = getaddrinfo(hostname.c_str(), NULL, &hints, &result);
    if (status != 0)
        return "Error: " + std::string(gai_strerror(status));
    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        if (rp->ai_family == AF_INET)
        {
            void *addr;
            char ipstr[INET_ADDRSTRLEN];

            struct sockaddr_in *ipv4 = (struct sockaddr_in *)rp->ai_addr;
            addr = &(ipv4->sin_addr);

            if (inet_ntop(rp->ai_family, addr, ipstr, sizeof(ipstr)) != NULL)
            {
                ipAddress = ipstr;
                break;
            }
        }
    }
    freeaddrinfo(result);
    return ipAddress;
}

int main()
{
    const std::string hostname = "google.m";
    std::string ipAddress = getIpv4Address(hostname);

    if (ipAddress.substr(0, 6) == "Error:")
    {
        std::cerr << "Error: " << ipAddress << std::endl;
    }
    else
    {
        std::cout << "IPv4 address: " << ipAddress << std::endl;
    }

    return 0;
}
