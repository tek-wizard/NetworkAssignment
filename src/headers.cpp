#include "headers.h"

using namespace std;

// id 0 means the name is sent as a literal
const char *names[] = {"", "host", "user-agent", "accept", "connection", "content-type",
                       "content-length", "server", "date", "last-modified", "cache-control"};

void putHeader(vector<uint8_t> &out, string name, string value) {
    int id = 0;
    for (int i = 1; i <= 10; i++)
        if (name == names[i]) id = i;
    out.push_back(id);
    if (id == 0) {
        out.push_back(name.size());
        out.insert(out.end(), name.begin(), name.end());
    }
    out.push_back(value.size() >> 8);
    out.push_back(value.size());
    out.insert(out.end(), value.begin(), value.end());
}

// returns the status code, or 0 if the payload is broken.
// the client does not need the headers, it only walks over them
int parseStatus(const vector<uint8_t> &p) {
    if (p.size() < 3) return 0;
    int status = (p[0] << 8) | p[1];
    int count = p[2];
    size_t pos = 3;
    for (int i = 0; i < count; i++) {
        if (pos >= p.size()) return 0;
        int id = p[pos++];
        if (id == 0) {
            if (pos >= p.size()) return 0;
            pos += 1 + p[pos];
        }
        if (pos + 2 > p.size()) return 0;
        pos += 2 + ((p[pos] << 8) | p[pos + 1]);
        if (pos > p.size()) return 0;
    }
    return status;
}
