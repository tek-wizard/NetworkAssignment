#ifndef REQUEST_H
#define REQUEST_H

#include <string>

bool sendRequest(int fd, std::string host, std::string path);
int readResponse(int fd);

#endif
