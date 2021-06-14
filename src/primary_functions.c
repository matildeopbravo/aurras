#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/stdprs.h"

/***************** PRIMAY FUNCTIONS *******************/

prs_pointer transform(int argc, char** argv) {
    printf("transform :)\n");

    if (argc > 0)
        for (int i = 0; i < argc; i++) printf("ARG[%d]: %s\n", i, argv[i]);

    char* code = malloc(sizeof(char) * 100);

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
    free(code);

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

