#include "../include/auxiliary.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../include/filtros.h"

ssize_t readch(int fd, char* buf) {
  static char file_buffer[BUF_SIZE];
  static int  next_position   = 0;
  static int  last_read_bytes = 0;
  int         bytes_read      = 0;

  if (next_position == last_read_bytes) {
    last_read_bytes = read(fd, file_buffer, BUF_SIZE);
    next_position   = 0;
  }
  if (last_read_bytes > 0) {
    *buf       = file_buffer[next_position++];
    bytes_read = 1;
  }
  return bytes_read;
}

ssize_t readln(int fd, char* line, size_t size) {
  int next_pos = 0;
  int r        = 0;
  while (next_pos < size && (r = readch(fd, line + next_pos)) == 1) {
    if (line[next_pos++] == '\n') break;
  }
  return r <= 0 ? 0 : next_pos;
}

ssize_t read_file(char* filename) {
  int  fd = open(filename, O_RDONLY);
  char line[BUF_SIZE];
  int  total_read = 0, len = 0;
  while ((len = readln(fd, line, BUF_SIZE)) > 0) {
    total_read += len;
    write(STDOUT_FILENO, line, len);
  }
  return total_read;
}

/* QUEUE */

Queue* init_queue(Request* request) {
  Queue* queue   = (Queue*) malloc(sizeof(struct queue));
  queue->request = request;
  queue->prox    = NULL;
  return queue;
}

void add_request_to_queue(Request* request, Queue* last_request) {
  if (last_request) {
    last_request->prox    = (Queue*) malloc(sizeof(struct queue));
    last_request          = last_request->prox;
    last_request->request = request;
  }
  else {
    last_request = init_queue(request);
  }
}

Request* remove_request(Queue* prev_queue, Queue* cur_queue) {
  // isto não deveria acontecer
  if (!prev_queue || !cur_queue) return NULL;

  Request* request = cur_queue->request;
  prev_queue->prox = cur_queue->prox;
  return request;
}

bool valid_request_to_execute(Request* request, CatalogoFiltros* catalogo) {
  bool   valid = true;
  size_t array[MAX_FILTER_NUMBER];

  for (size_t i = 0; i < request->number_filters; i++)
    array[request->requested_filters[i]]++;

  for (size_t i = 0; valid && i < request->number_filters; i++)
    valid = array[i] < catalogo->filtros[i]->max_instancias;
  return valid;
}

// TODO verificar
Request* can_execute_request(
    Queue*           queue,
    CatalogoFiltros* catalogo,
    Queue*           proximo_endereco_a_analisar) {
  Request* request  = NULL;
  Queue*   endereco = queue;
  bool     find     = false;

  while (endereco && !find) {
    bool     fail    = false;
    Request* request = queue->request;
    find             = valid_request_to_execute(request, catalogo);
    if (!find) endereco = endereco->prox;
  }
  if (find) {
    request                     = remove_request(endereco, endereco->prox);
    proximo_endereco_a_analisar = endereco->prox;
  }
  else
    proximo_endereco_a_analisar = NULL;

  return request;
}
