#ifndef PRIMARY_FUNCTIONS_H
#define PRIMARY_FUNCTIONS_H

#include "stdprs.h"
prs_pointer transform(int argc, char** argv);
prs_pointer status(int argc, char** argv);
prs_pointer info(int argc, char** argv);
void show_error(_ERROR error);

#endif

