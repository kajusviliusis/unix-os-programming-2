#ifndef PIPELINE_H
#define PIPELINE_H

#include "command.h"

int execute_command(const Command *command);
int execute_pipeline(const Command *commands, size_t count);

#endif
