#include "../../Include/Client/Client.hpp"

Client::Client(int fd) : fd(fd), read(true), write(false) {}

Client& Client::operator=(const Client& overl)
{
    fd = overl.fd;
    request = overl.request;
    read = overl.read;
    write = overl.write;
    return(*this);
}

Client::Client(const Client& copy)  {*this = copy;}

Client::~Client() {}

std::string Response(const Client& clients) ///
{
    std::string path, holdFile, body;
    std::stringstream stream, stream1;
    std::cout << clients.request.URI << std::endl;
    body =  "HTTP/1.1 200 OK\r\nContent-Length: ";//318\r\nContent-Type: text/html\r\n\r\n";
    if (clients.request.ReqstDone == true && clients.request.URI == "/")
        path = "www/selec.html";
    else if (clients.request.ReqstDone == ERROR)
    {
        path = "www/error.html";
        body =  "HTTP/1.1 400 Bad Request\r\nContent-Length: ";//318\r\nContent-Type: text/html\r\n\r\n";

    }
    else
        path = "www/vide.html";

    std::ifstream index_html(path.c_str());
    if (!index_html.is_open())
        std::cout << "error \n"; // handle response
    stream << index_html.rdbuf();
    holdFile = stream.str();

    stream1 << holdFile.size();
    body += stream1.str();
    body += "\r\nContent-Type: text/html\r\n\r\n";
    body += holdFile;

    // std::ofstream outf("mqawed",std::ios::out | std::ios::trunc);
    // outf << body;


    return body;
}
