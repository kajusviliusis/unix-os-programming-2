#ifndef PARSER_H
#define PARSER_H

#include "command.h"

int parse_command(const char *input, Command *command);
int parse_pipeline(const char *input, Command **commands, size_t *count);

#endif
