#ifndef REQUEST_H
#define REQUEST_H
#include <stdio.h>
#include <stdlib.h>

#define MAX_FILTER_NUMBER 32
#define MAX_FILENAME_LEN 128

typedef enum request_type { TRANSFORM, STATUS } RequestType;

typedef struct request {
    pid_t client_pid;
    RequestType request_type;
    int requested_filters[MAX_FILTER_NUMBER];
    char input_file[MAX_FILENAME_LEN];
    char output_file[MAX_FILENAME_LEN];
} Request;

#endif

