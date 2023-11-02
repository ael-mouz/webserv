#pragma once

#include "../Server/Utils.hpp"
#include "ChunkedEncoding.hpp"
#include "Headers.hpp"
#include "Multipart.hpp"
#include "RequestLine.hpp"

class Client;

enum _mainState {
    ERROR,
    REQUEST_LINE,
    HEADERS,
    BODY,
    END,
};

struct File {
    string fileName;
    multimap<string, string> Content;
};

class Request_Fsm {
  private:
    RequestLine requestLine;
    Headers headers;
    Multipart multi;
    ChunkedEncoding decode;

  public:
    int mainState;
    int subState;
    string hold;
    string key;
    string boundary;
    bool decodeFlag;
    int sizeBoundary;
    size_t ContentLength;
    int ReqstDone;
    string Method;
    string URI;
    multimap<string, string> mapHeaders;
    vector<File> files;
    void read(Client &client, string &buffer, ssize_t &size);
    void clear(void);
    Request_Fsm();
    ~Request_Fsm();
};
