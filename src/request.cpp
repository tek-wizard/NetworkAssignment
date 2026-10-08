#include <iostream>
#include <string>
#include <vector>
#include "request.h"
#include "frame.h"
#include "headers.h"
#include "dump.h"

using namespace std;

bool sendRequest(int fd, string host, string path) {
    vector<uint8_t> p;
    p.push_back(1);
    p.push_back(path.size() >> 8);
    p.push_back(path.size());
    p.insert(p.end(), path.begin(), path.end());
    p.push_back(4);
    putHeader(p, "host", host);
    putHeader(p, "user-agent", "bcurl/1.0");
    putHeader(p, "accept", "*/*");
    putHeader(p, "connection", "keep-alive");

    uint32_t len = p.size();
    vector<uint8_t> f;
    f.push_back(MAGIC);
    f.push_back(VERSION);
    f.push_back(T_REQUEST);
    f.push_back(0);
    f.push_back(len >> 24);
    f.push_back(len >> 16);
    f.push_back(len >> 8);
    f.push_back(len);
    f.insert(f.end(), p.begin(), p.end());

    if (verbose) dumpFrame(">", f);
    return writeFull(fd, f.data(), f.size());
}

// returns the status code, or 0 if something went wrong
int readResponse(int fd) {
    int status = 0;
    while (true) {
        vector<uint8_t> f(8);
        if (!readFull(fd, f.data(), 8)) return 0;
        if (f[0] != MAGIC || f[1] != VERSION) return 0;
        uint32_t len = ((uint32_t)f[4] << 24) | (f[5] << 16) | (f[6] << 8) | f[7];
        f.resize(8 + len);
        if (len > 0 && !readFull(fd, &f[8], len)) return 0;
        if (verbose) dumpFrame("<", f);

        uint8_t type = f[2];
        uint8_t flags = f[3];
        vector<uint8_t> payload(f.begin() + 8, f.end());
        if (type == T_RESPONSE) {
            status = parseStatus(payload);
            if (status == 0) return 0;
        } else if (type == T_BODY) {
            cout.write((char *)payload.data(), payload.size());
            if (flags & F_LAST) {
                cout.flush();
                return status;
            }
        }
        // any other type is skipped, its payload is already read
    }
}
