#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../include/request.h"
#include "../include/stdprs.h"

/***************** MACROS *****************************/

#define INPUT_FILE_ARGV 0
#define OUTPUT_FILE_ARGV 1
#define FIRST_FILTER_ARGV 2

/***************** HELPERS ****************************/

/* return -1 if filter does not exist (otherwise return the filter_index for
 * Request)*/
int parser_filter(char* test_filter) {
    // TODO ver configuração
    if (!strcmp(test_filter, "alto")) return 0;
    if (!strcmp(test_filter, "baixo")) return 1;
    if (!strcmp(test_filter, "eco")) return 2;
    if (!strcmp(test_filter, "rapido")) return 3;
    if (!strcmp(test_filter, "lento")) return 4;
    return -1;
}

/* return the argv postion of the filter that does not exist (otherwise return
 * -1) */
int parser_filters(Request* request, char* argv[], int argc) {
    for (int i = 0; i < MAX_FILTER_NUMBER; i++) {
        request->requested_filters[i] = 0;
    }

    for (int i = FIRST_FILTER_ARGV; i < argc; i++) {
        int filter_postion = parser_filter(argv[i]);

        /* the filter does not exist */
        if (filter_postion == -1) return i;

        request->requested_filters[filter_postion]++;
    }
    return -1;
}

void parser_filenames(Request* request, char* argv[], int argc) {
    strcpy(request->input_file, argv[INPUT_FILE_ARGV]);
    strcpy(request->output_file, argv[OUTPUT_FILE_ARGV]);
}

/***************** PRIMAY FUNCTIONS *******************/

prs_pointer transform(int argc, char** argv) {
    printf("transform :)\n");

    if (argc > 0)
        for (int i = 0; i < argc; i++) printf("ARG[%d]: %s\n", i, argv[i]);

    Request request;
    request.request_type = TRANSFORM;

    /* filters parser */
    parser_filenames(&request, argv, argc);

    /* filenames parser */
    int invalid_filter;

    if ((invalid_filter = parser_filters(&request, argv, argc)) != -1) {
        printf(
            "Filter \"%s\" does not exist. Please insert a valid filter\n",
            argv[invalid_filter]);
        return NULL;
    }

    /* create a child to send a pid to server */
    int pid_client;
    if ((pid_client = fork()) == 0) {
        request.client_pid = getpid();

        int fd = open("client_to_server", O_WRONLY);
        write(fd, &request, sizeof(struct request));
        _exit(0);
    }

    int status;
    wait(&status);

    return NULL;
}

prs_pointer status(int argc, char** argv) {
    printf("status :)\n");

    char* code = "$status%";
    printf("\nString to <write>: %s\n", code);

    return NULL;
}

prs_pointer info(int argc, char** argv) {
    printf("info :)\n");

    return NULL;
}

void show_error(_ERROR error) {
    if (error == COMMAND) {
        printf("Incorrect command name\n");
        return;
    }
    if (error == N_ARGS) {
        printf("Incorrect number of arguments\n");
        return;
    }
}

