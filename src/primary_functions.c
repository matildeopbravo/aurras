#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../include/reply.h"
#include "../include/request.h"
#include "../include/stdprs.h"

/***************** MACROS *****************************/

#define INPUT_FILE_ARGV 0
#define OUTPUT_FILE_ARGV 1
#define FIRST_FILTER_ARGV 2

/***************** HELPERS ****************************/

void show_state(State state, State last_state) {

    switch (state) {
        case PENDING:
            write(STDOUT_FILENO, "pending\n", sizeof(char) * 9);
            break;
        case PROCESSING:
            write(STDOUT_FILENO, "processing\n", sizeof(char) * 12);
            break;
        case FINISHED:
            write(STDOUT_FILENO, "finished\n", sizeof(char) * 10);
            return;
            break;
        case NOTHING:
            break;
        default:
            break;
    }
}

/* return -1 if filter does not exist (otherwise return the filter_index for
 * Request)*/
int parser_filter(char* test_filter, char* all_filters) {
    char* copy = strdup(all_filters);
    char* init_copy = copy;
    char* token = strsep(&copy, ";");
    int filter_index = 0;

    while (token != NULL) {
        if (!strcmp(test_filter, token)) {
            free(init_copy);
            return filter_index;
        }

        token = strsep(&copy, ";");
        filter_index++;
    }

    free(init_copy);
    return -1;
}

/* return the argv postion of the filter that does not exist (otherwise return
 * -1) */
int parser_filters(
    Request* request, char* argv[], int argc, char* all_filters) {

    all_filters[strlen(all_filters) - 1] = '\0';

    for (int i = 0; i < MAX_FILTER_NUMBER; i++) {
        request->requested_filters[i] = -1;
    }

    size_t number_filters = 0;
    for (int i = FIRST_FILTER_ARGV; i < argc; i++) {
        int filter_postion = parser_filter(argv[i], all_filters);
        /* the filter does not exist */
        if (filter_postion == -1) return i;
        request->requested_filters[i - FIRST_FILTER_ARGV] = filter_postion;
        number_filters++;
    }
    request->number_filters = number_filters;
    return -1;
}

void parser_filenames(Request* request, char* argv[], int argc) {
    strcpy(request->input_file, argv[INPUT_FILE_ARGV]);
    strcpy(request->output_file, argv[OUTPUT_FILE_ARGV]);
}

char* handshake() {

    Request handshake = {.client_pid = getpid(), .request_type = HANDSHAKE};

    /***** create the server_to_client pipe to get the Reply *****/
    char server_to_client_fifo_name[1024];
    sprintf(server_to_client_fifo_name, "tubo_%d", handshake.client_pid);
    mkfifo(server_to_client_fifo_name, 0644);

    /*************** send the request to server *****************/
    int client_to_server = open("client_to_server", O_WRONLY);
    write(client_to_server, &handshake, sizeof(struct request));

    /****** open the reply_pipe to read the server  reply *******/
    int server_to_client = open(server_to_client_fifo_name, O_RDONLY);

    char* filters = (char*) malloc(sizeof(char) * BUFSIZ);
    read(server_to_client, filters, BUFSIZ);
    /* finished handshake */
    return filters;
}

/***************** PRIMAY FUNCTIONS *******************/

prs_pointer transform(int argc, char** argv) {
    char* filters = handshake();

    Request request = {.request_type = TRANSFORM, .client_pid = getpid()};

    /* filters parser */
    parser_filenames(&request, argv, argc);

    /* filenames parser */
    int invalid_filter;

    if ((invalid_filter = parser_filters(&request, argv, argc, filters)) !=
        -1) {
        char buffer[32];
        int n =
            sprintf(buffer, "Filter %s does not exist\n", argv[invalid_filter]);
        write(STDERR_FILENO, buffer, n);
        return NULL;
    }

    /* create a child to send a pid to server */

    /***** create the server_to_client pipe to get the Reply *****/
    char server_to_client_fifo_name[1024];
    sprintf(server_to_client_fifo_name, "tubo_%d", request.client_pid);
    mkfifo(server_to_client_fifo_name, 0644);

    /*************** send the request to server *****************/
    int client_to_server = open("client_to_server", O_WRONLY);
    write(client_to_server, &request, sizeof(struct request));

    /****** open the reply_pipe to read the server  reply *******/
    /* OBS: only now I can do this because if I open before the request
     * the server can't open the pipe (becausa de open here will be blocked)
     * and the process will break before the request */
    int server_to_client = open(server_to_client_fifo_name, O_RDONLY);
    open(server_to_client_fifo_name, O_WRONLY);

    Reply reply;
    State last_state = NOTHING;
    int load_pid = -1;
    while (read(server_to_client, &reply, sizeof(struct reply)) > 0) {
        show_state(reply.state, last_state);
        if (reply.state == FINISHED) {
            break;
        }
    }
    unlink(server_to_client_fifo_name);
    return NULL;
}

prs_pointer status(int argc, char** argv) {
    printf("status :)\n");

    return NULL;
}

prs_pointer info(int argc, char** argv) {
    char buffer[128];
    int n = sprintf(
        buffer,
        "./aurras status\n./aurras transform input-filename output-filename "
        "filter-id-1 filter-id-2 ...\n");
    write(STDOUT_FILENO, buffer, n);

    return NULL;
}

void show_error(_ERROR error) {
    char buffer[32];
    int n = 0;
    if (error == COMMAND) {
        n = sprintf(buffer, "Incorrect command name\n");
        write(STDERR_FILENO, buffer, n);
        return;
    }
    if (error == N_ARGS) {
        n = sprintf(buffer, "Incorrect number of args\n");
        write(STDERR_FILENO, buffer, n);
        return;
    }
}

