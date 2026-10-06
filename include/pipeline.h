#ifndef PIPELINE_H
#define PIPELINE_H

#include "command.h"

int execute_command(const Command *command);
int execute_two_command_pipeline(const Command *commands);

#endif
