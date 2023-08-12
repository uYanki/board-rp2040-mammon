#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

typedef enum {
    SHELL_SUCCESS,
    SHELL_ERROR_UNKNOWN,
    SHELL_ERROR_NULL_PTR,
    SHELL_ERROR_CMD_NOT_FOUND,
    SHELL_ERROR_INVALID_ARGS,
    SHELL_ERROR_BUFFER_FULL,
    SHELL_IDLE,
} shell_status_t;

/*!
 * @brief Function type declarations.
 */
typedef shell_status_t (*cbk_cmd_t)(int argc, char** argv);
typedef void (*cbk_println_t)(const char* string);

/*!
 * @brief Command structure, consisting of a name and function pointer.
 */
typedef struct {
    const char* cmd;   //  Command name.
    cbk_cmd_t   func;  //  Function pointer to associated function.
    const char* help;  //  help/usage summary
} cmd_t;

/*!
 * @brief Command-line interface handle structure.
 */
typedef struct {
    cbk_println_t println;  // Function pointer to user defined println function.
    cmd_t*        cmd_tbl;  // Pointer to series of commands which are to be accepted.
    size_t        cmd_cnt;  // Number of commands in cmd_tbl.
} shell_t;

#ifdef __cplusplus
}
#endif