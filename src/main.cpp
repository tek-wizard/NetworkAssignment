#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include "dump.h"
#include "request.h"

using namespace std;

int main(int argc, char **argv) {
    vector<string> args;
    for (int i = 1; i < argc; i++) {
        if (string(argv[i]) == "-v") verbose = true;
        else args.push_back(argv[i]);
    }
    if (args.empty()) {
        cerr << "usage: " << argv[0] << " [-v] host:port/path [/more/paths...]" << endl;
        return 2;
    }

    string url = args[0];
    if (url.compare(0, 7, "http://") == 0) url = url.substr(7);
    string hostport = url;
    string path = "/";
    size_t slash = url.find('/');
    if (slash != string::npos) {
        hostport = url.substr(0, slash);
        path = url.substr(slash);
    }
    string host = hostport;
    string port = "80";
    size_t colon = hostport.find(':');
    if (colon != string::npos) {
        host = hostport.substr(0, colon);
        port = hostport.substr(colon + 1);
    }

    vector<string> paths;
    paths.push_back(path);
    for (size_t i = 1; i < args.size(); i++) paths.push_back(args[i]);

    addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(host.c_str(), port.c_str(), &hints, &res) != 0) {
        cerr << "bcurl: cannot resolve " << host << endl;
        return 2;
    }
    int fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (connect(fd, res->ai_addr, res->ai_addrlen) < 0) {
        perror("bcurl: connect");
        return 2;
    }
    freeaddrinfo(res);

    // every request goes over this one connection
    int exitCode = 0;
    for (size_t i = 0; i < paths.size(); i++) {
        if (!sendRequest(fd, hostport, paths[i])) {
            cerr << "bcurl: send failed" << endl;
            return 2;
        }
        int status = readResponse(fd);
        if (status == 0) {
            cerr << "bcurl: bad or missing response" << endl;
            return 2;
        }
        if (status >= 400) {
            cerr << "bcurl: " << paths[i] << " returned " << status << endl;
            exitCode = 1;
        }
    }
    close(fd);
    return exitCode;
}
