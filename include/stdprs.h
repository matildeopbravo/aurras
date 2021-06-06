#ifndef STDPRS_H
#define STDPRS_H

/**************** TYPEDEF'S **********************/

typedef void * prs_pointer;
typedef struct _command * _COMMAND;
typedef struct _commands_table * _COMMANDS_TABLE;
typedef struct _parser_info * _PARSER_INFO;

typedef enum status {
	ERROR = 1,
	SUCCESSFUL = 2,
}	_STATUS;

typedef enum error {
	COMMAND = 1,
	N_ARGS = 2,
}	_ERROR;

typedef enum args_quantity {
	EQUALS = 1,
	GREATER = 2,
}	_ARGS_QUANTITY;

/**************** FUNCTIONS *********************/

_COMMANDS_TABLE init_stdprs();
_COMMAND create_command(char * name, int argc, prs_pointer (*func) (int argc, char ** argv), _ARGS_QUANTITY);
void insert_command(_COMMANDS_TABLE c_t, _COMMAND command);
_PARSER_INFO parser_cmd(_COMMANDS_TABLE c_t, int argc, char ** argv);
void do_command(_PARSER_INFO p_f);
_STATUS get_status(_PARSER_INFO);
_ERROR get_error(_PARSER_INFO);
void destroy_stdprs(_COMMANDS_TABLE);

#endif
