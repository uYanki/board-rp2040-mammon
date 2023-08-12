#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "shell_defs.h"

shell_status_t shell_cycle();

void shell_init(void);
void shell_put(char c);

extern shell_t shell;

#ifdef __cplusplus
}
#endif
