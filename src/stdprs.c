#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../include/stdprs.h"

/***************** MACROS *****************************/

#define DEFAULT_N_COMMANDS 10

/***************** STRUCTS ****************************/

struct _command{
	char * command_name; 
	int command_n_args; /* number of arguments (-1 = ONLY exec) */
	_ARGS_QUANTITY args_quantity;
	prs_pointer (* func) (int argc, char ** args);
};

struct _commands_table{
	_COMMAND * table;
	int n_commands;
	int size;
};

struct _parser_info{
	prs_pointer (*func) (int argc, char ** args);
	char ** args;
	int n_args;
	_STATUS status;
	_ERROR error;
};

/*************** INIT FUNCTIONS *********************/

_COMMANDS_TABLE init_stdprs(){
	_COMMANDS_TABLE c_t = malloc(sizeof(struct _commands_table));
	c_t->table = malloc(sizeof(_COMMAND) * DEFAULT_N_COMMANDS);
	c_t->n_commands = 0;
	c_t->size = DEFAULT_N_COMMANDS;

	return c_t;
}

/*************** DESTROY FUNCTIONS *****************/

static void destroy_command(_COMMAND c){
	free(c);
}

void destroy_stdprs(_COMMANDS_TABLE c_t){
	int n_commands = c_t->n_commands;

	for (int i = 0; i < n_commands; i++){
		destroy_command(c_t->table[i]);
	}

	free(c_t);
}

/*************** GETTER'S ****************************/

_STATUS get_status(_PARSER_INFO parser_info){
	return parser_info->status;
}

_ERROR get_error(_PARSER_INFO parser_info){
	return parser_info->error;
}

/*************** DUPLICATE FUNCTIONS *****************/

_COMMAND dup_command(_COMMAND command){
	_COMMAND cmd = malloc(sizeof(_COMMAND));

	cmd->command_n_args = command->command_n_args;
	cmd->command_name = strdup(command->command_name);
	cmd->func = command->func;
	cmd->args_quantity = command->args_quantity;

	return cmd;
}

/*************** CREATE FUNCTIONS ********************/

_COMMAND create_command(char * name, int argc, prs_pointer (*func) (int argc, char ** argv), _ARGS_QUANTITY q){
	_COMMAND cmd = malloc(sizeof(_COMMAND));

	cmd->command_n_args = argc;
	cmd->command_name = strdup(name);
	cmd->func = func;
	cmd->args_quantity = q;

	return cmd;
}

/*************** INSERT FUNCTIONS ********************/

void insert_command(_COMMANDS_TABLE c_t, _COMMAND command){
	
	if (c_t->n_commands == c_t->size) {
		c_t->size *= 2;
		c_t->table = malloc(sizeof(_COMMAND) * c_t->size);
	}

	c_t->table[c_t->n_commands++] = (_COMMAND) dup_command(command);
		
}

/**************** PARSER FUNCTIONS *******************/

_PARSER_INFO parser_cmd(_COMMANDS_TABLE c_t, int argc, char ** argv){
	_PARSER_INFO prs_info = malloc(sizeof(struct _parser_info));
	prs_info->args = malloc(sizeof(char *));
	prs_info->status = ERROR;

	for (int i = 0; i < c_t->n_commands; i++){
		if ((argc > 1 && !strcmp(argv[1], c_t->table[i]->command_name))
				|| (c_t->table[i]->command_n_args == -1 && argc == 1)){
		
			/* check if function have the correct argument numbers */
			/******************************************************/
			if(c_t->table[i]->args_quantity == EQUALS &&
					argc - 2 != c_t->table[i]->command_n_args){
				prs_info->status = ERROR;
				prs_info->error = N_ARGS;
				return prs_info;
			}

			if (c_t->table[i]->args_quantity == GREATER &&
					argc - 2 < c_t->table[i]->command_n_args){
				prs_info->status = ERROR;
				prs_info->error = N_ARGS;
				return prs_info;
			}
			/******************************************************/
			
			if (c_t->table[i]->command_n_args > 0) prs_info->n_args = argc - 2;
			else prs_info->n_args = 0;

			prs_info->status = SUCCESSFUL;
			prs_info->func = c_t->table[i]->func;
			
			prs_info->args = malloc(sizeof(char *) * (argc - 2));
			for (int i = 2; i < argc; i++) prs_info->args[i - 2] = strdup(argv[i]);

			return prs_info;
		}
	}

	prs_info->error = COMMAND;
	return prs_info;
}


/***************** REGULAR FUNCTIONS ******************/

void do_command(_PARSER_INFO p_f){
	p_f->func(p_f->n_args, p_f->args);
}
