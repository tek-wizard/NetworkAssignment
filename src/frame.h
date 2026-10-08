#ifndef FRAME_H
#define FRAME_H

#include <cstdint>
#include <cstddef>

const uint8_t MAGIC = 'B';
const uint8_t VERSION = 1;
const uint8_t T_REQUEST = 1;
const uint8_t T_RESPONSE = 2;
const uint8_t T_BODY = 3;
const uint8_t F_LAST = 1;

bool readFull(int fd, uint8_t *buf, size_t n);
bool writeFull(int fd, const uint8_t *buf, size_t n);

#endif
