#include <iomanip>
#include <iostream>
#include "dump.h"
#include "frame.h"

using namespace std;

bool verbose = false;

static void hexdump(const vector<uint8_t> &d) {
    for (size_t i = 0; i < d.size(); i += 16) {
        cerr << "  " << hex << setfill('0') << setw(4) << i << "  ";
        for (size_t j = i; j < i + 16; j++) {
            if (j < d.size()) cerr << setw(2) << (int)d[j] << " ";
            else cerr << "   ";
        }
        cerr << " |";
        for (size_t j = i; j < i + 16 && j < d.size(); j++)
            cerr << (char)((d[j] >= 32 && d[j] < 127) ? d[j] : '.');
        cerr << "|" << endl;
    }
    cerr << dec << setfill(' ');
}

void dumpFrame(string dir, const vector<uint8_t> &f) {
    string name = "UNKNOWN";
    if (f[2] == T_REQUEST) name = "REQUEST";
    if (f[2] == T_RESPONSE) name = "RESPONSE";
    if (f[2] == T_BODY) name = "BODY";
    uint32_t len = ((uint32_t)f[4] << 24) | (f[5] << 16) | (f[6] << 8) | f[7];
    cerr << dir << " " << name << " frame: type=" << (int)f[2] << " flags=" << (int)f[3]
         << " len=" << len << endl;
    hexdump(f);
}
