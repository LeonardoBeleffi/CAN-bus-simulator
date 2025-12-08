#ifndef _TERMINAL_LAUNCHER_H
#define _TERMINAL_LAUNCHER_H

#include <sys/types.h>

pid_t launch_in_new_terminal(const char *program_path, char *program_args[]);

#endif

