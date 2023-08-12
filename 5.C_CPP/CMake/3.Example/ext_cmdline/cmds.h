#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "shell_defs.h"

extern cmd_t cmds[];
extern shell_status_t rslt;
extern shell_t shell;

void shell_println(const char * string);

#ifdef __cplusplus
}
#endif