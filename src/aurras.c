#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../include/primary_functions.h"
#include "../include/request.h"
#include "../include/stdprs.h"

int main(int argc, char** argv) {
    Request novo = {
        .client_pid = getpid(),
        .request_type = TRANSFORM,
        .requested_filters = {1, 0, 2},
        .input_file = "sou_linda.mp4",
        .output_file = "sou_linda_output.mp4"};
    int fd_leitura = open("client_to_server", O_RDONLY);
    write(fd_leitura, &novo, sizeof(novo));

    /*************** INIT CMD TABLE *****************************/
    _COMMANDS_TABLE cmd_t = init_stdprs();

    /*************** CREATE COMMANDS *****************************/
    _COMMAND cmd1 = create_command("transform", 3, transform, GREATER);
    _COMMAND cmd2 = create_command("status", 0, status, EQUALS);
    _COMMAND cmd3 = create_command("info", -1, info, EQUALS);

    /*************** INSERT COMMANDS *****************************/
    insert_command(cmd_t, cmd1);
    insert_command(cmd_t, cmd2);
    insert_command(cmd_t, cmd3);

    /************** PARSER and EXECUTION ************************/
    _PARSER_INFO p_i = parser_cmd(cmd_t, argc, argv);
    if (get_status(p_i) != ERROR)
        do_command(p_i);
    else
        show_error(get_error(p_i));

    destroy_stdprs(cmd_t);
    return 0;
}

