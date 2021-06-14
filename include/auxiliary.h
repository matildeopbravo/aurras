#ifndef AUXILIARY_H
#define AUXILIARY_H

#include <stdlib.h>

#define BUF_SIZE 1024

ssize_t readch(int fd, char* buf);

ssize_t readln(int fd, char* line, size_t size);

ssize_t read_file(char* filename);
#endif
