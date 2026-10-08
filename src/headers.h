#ifndef HEADERS_H
#define HEADERS_H

#include <cstdint>
#include <string>
#include <vector>

void putHeader(std::vector<uint8_t> &out, std::string name, std::string value);
int parseStatus(const std::vector<uint8_t> &p);

#endif
