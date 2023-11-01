#include "../../include/Server/Client.hpp"

// Client::Client(ServerConf& serverConf, int socketClient) : socketClient(socketClient), serverConf(serverConf), request(serverConf)
Client::Client(int socketClient) : socketClient(socketClient)//, serverConf(serverConf), request(serverConf)
{
    // (void)serverConf;
    read = true;
    write = false;
}

Client& Client::operator=(const Client& overl)
{
    socketClient = overl.socketClient;
    request = overl.request;
    read = overl.read;
    write = overl.write;
    return(*this);
}

Client::Client(const Client& copy)// : serverConf(copy.serverConf), request(copy.serverConf)
{
    *this = copy;
}

Client::~Client() {}

std::string _Response(const Client &clients) ///
{
    std::string path, holdFile, body;
    std::stringstream stream, stream1;
    std::cout << clients.request.URI << std::endl;
    body =  "HTTP/1.1 200 OK\r\nContent-Length: ";//318\r\nContent-Type: text/html\r\n\r\n";
    if (clients.request.ReqstDone == 200 && clients.request.URI == "/")
        path = "www/selec.html";
    else if (clients.request.ReqstDone == 400)
    {
        path = "www/error.html";
        body =  "HTTP/1.1 400 Bad Request\r\nContent-Length: ";//318\r\nContent-Type: text/html\r\n\r\n";

    }
    else
        path = "www/vide.html";

    std::ifstream index_html(path.c_str());
    if (!index_html.is_open())
        printf("_Response is_open error path = %s\n", &path[0]); // handle response
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
