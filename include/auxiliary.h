#ifndef AUXILIARY_H
#define AUXILIARY_H

#include <stdlib.h>

#include "request.h"

#define BUF_SIZE 1024

typedef struct queue {
  Request*      request;
  struct queue* prox;
} Queue;

ssize_t readch(int fd, char* buf);

ssize_t readln(int fd, char* line, size_t size);

ssize_t read_file(char* filename);

Queue* init_queue(Request* request);

void add_request_to_queue(Request* request, Queue* last_request);

Request* remove_request(Queue* prev_queue, Queue* cur_queue);
#endif
