#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>

#include "../include/reply.h"
#include "../include/request.h"
#include "../include/stdprs.h"

/***************** MACROS *****************************/

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

#define INPUT_FILE_ARGV 0
#define OUTPUT_FILE_ARGV 1
#define FIRST_FILTER_ARGV 2

/***************** HELPERS ****************************/

void show_state (State state, State last_state) {

    switch(last_state){
        case PENDING: 
      printf (ANSI_COLOR_GREEN "\r→ "ANSI_COLOR_RESET "pending " ANSI_COLOR_GREEN "complete\n" ANSI_COLOR_RESET);
            break;
        case PROCESSING:
    printf (ANSI_COLOR_GREEN "\r→ "ANSI_COLOR_RESET "processing " ANSI_COLOR_GREEN "complete\n" ANSI_COLOR_RESET);
            break;
        case FINISHED:
            return;
        case NOTHING:
            break;
        default:
            printf("Default\n");
            break;
    }

    switch(state){
        case PENDING: 
            printf (ANSI_COLOR_BLUE "→" ANSI_COLOR_RESET " pending ");
            break;
        case PROCESSING:
            printf (ANSI_COLOR_BLUE "→" ANSI_COLOR_RESET " processing ");
            break;
        case FINISHED:
            printf (ANSI_COLOR_GREEN "→" ANSI_COLOR_RESET " finished\n ");
            return;
            break;
        case NOTHING:
            break;
        default:
            printf("Default\n");
            break;
    }
    
    fflush (stdout);
  
  while (1) {  
    printf (ANSI_COLOR_YELLOW "." ANSI_COLOR_RESET);
    fflush (stdout);
    sleep (1);
    
    printf (ANSI_COLOR_YELLOW "." ANSI_COLOR_RESET);
    fflush (stdout);
    sleep (1);
    
    fflush (stdout);
    printf (ANSI_COLOR_YELLOW "." ANSI_COLOR_RESET);
    fflush (stdout);
    sleep(1);
    
    printf ("\033[3D");
    fflush (stdout);
    printf ("\033[K");
    fflush (stdout);
    sleep (1);
  }
}

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

    size_t number_filters = 0;
    for (int i = FIRST_FILTER_ARGV; i < argc; i++) {
        int filter_postion = parser_filter(argv[i]);
        /* the filter does not exist */
        if (filter_postion == -1) return i;
        request->requested_filters[filter_postion]++;
        number_filters++;
    }
    request->number_filters = number_filters;
    return -1;
}

void parser_filenames(Request* request, char* argv[], int argc) {
    strcpy(request->input_file, argv[INPUT_FILE_ARGV]);
    strcpy(request->output_file, argv[OUTPUT_FILE_ARGV]);
}

/***************** PRIMAY FUNCTIONS *******************/

prs_pointer transform(int argc, char** argv) {

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
        open(server_to_client_fifo_name,O_WRONLY); 

        Reply reply;
        State last_state = NOTHING;
        int load_pid = -1;
        while(read(server_to_client, &reply, sizeof(struct reply)) > 0){
            
            if (load_pid != -1){
                kill(load_pid, SIGKILL);
            }
           
            if (reply.state == FINISHED) {show_state(reply.state, last_state); _exit(0);}
            
            if ((load_pid = fork()) == 0){show_state(reply.state, last_state);_exit(0);}
            last_state = reply.state;
        }

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
    printf("./aurras status\n./aurras transform input-filename output-filename filter-id-1 filter-id-2 ...\n");

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

