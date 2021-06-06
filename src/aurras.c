#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/stdprs.h"

/***************** PRIMAY FUNCTIONS *******************/

prs_pointer transform(int argc, char ** argv){
	printf("transform :)\n");
	
	if (argc > 0) for (int i = 0; i < argc; i++) printf("ARG[%d]: %s\n", i, argv[i]);

	char * code = malloc(sizeof(char) * 100);
	
	/* START char */
	code[0] = '$';
	strcat(code, "transform");

	for (int i = 0; i < argc; i++) {
		
		/* ARGS char */
		strcat(code, "#");
		strcat(code, argv[i]);
	}
	
	/* END char */
	strcat(code, "%");

	printf("\nString to <write>: %s\n", code);
	
	return NULL;
}

prs_pointer status(int argc, char ** argv){
	printf("status :)\n");
	
	char *code = "$status%";
	printf("\nString to <write>: %s\n", code);
	
	return NULL;
}

prs_pointer info(int argc, char ** argv){
	printf("info :)\n");
	
	return NULL;
}

void show_error(_ERROR error){
	if (error == COMMAND) {printf("Incorrect command name\n");return;}
	if (error == N_ARGS) {printf("Incorrect number of arguments\n");return;}
}

int main(int argc, char ** argv){
	
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
	if (get_status(p_i) != ERROR) do_command(p_i); else show_error(get_error(p_i));
	
	destroy_stdprs(cmd_t);
	return 0;
	
}

