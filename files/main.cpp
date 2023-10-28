#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>

bool isValidIPAddress(const std::string &ipAddress)
{
    std::vector<std::string> parts;
    std::string part;
    std::istringstream ss(ipAddress);
    while (std::getline(ss, part, '.'))
        parts.push_back(part);
    if (parts.size() != 4)
    {
        std::cout << "Invalid IP address: " << ipAddress << std::endl;
        return false;
    }
    for (size_t i = 0; i < parts.size(); ++i)
    {
        const std::string &part = parts[i];
        char *end;
        long num = std::strtol(part.c_str(), &end, 10);
        std::cout << "num " << num << std::endl;
        std::cout << "end " << end << std::endl;
        if (*end != '\0' || num < 0 || num > 255)
        {
            std::cout << "Invalid IP address: " << ipAddress << std::endl;
            return false;
        }
    }
    std::cout << "Valid IP address: " << ipAddress << std::endl;
    return true;
}

int main()
{
    std::string ipAddress = "192.168.1.g151515d";

    if (isValidIPAddress(ipAddress))
    {
        std::cout << "Valid IP address: " << ipAddress << std::endl;
    }
    else
    {
        std::cout << "Invalid IP address: " << ipAddress << std::endl;
    }

    return 0;
}
