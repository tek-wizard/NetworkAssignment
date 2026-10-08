#ifndef DUMP_H
#define DUMP_H

#include <cstdint>
#include <string>
#include <vector>

extern bool verbose;

void dumpFrame(std::string dir, const std::vector<uint8_t> &f);

#endif
