#include "terminal_launcher.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

// (Null-terminated) List of supported terminals
static char *supported_terminals[][2] = {
	// Tested
	{"konsole", "-e"},
	{"kitty", "-e"},
	{NULL, NULL}

	// Not yet tested
	// {"gnome-terminal", "--"},
	// {"xfce4-terminal", "-e"},
	// {"xterm", "-e"},
	// {"terminator", "-e"},
};

/* This function uses the `which` shell built-in to check if the terminal is
 * listed in the PATH environment variable. It returns 1 if the termainal
 * exists on the system, 0 otherwise.
 */
static int terminal_exists(const char* terminal_index) {
		char command[256];
		snprintf(command, sizeof(command), "which %s > /dev/null 2>&1", terminal_index);

		return system(command) == 0;
}

/* This function tries to find an installated terminal emulator between the
 * ones present in the list above. It returns the index of the first found
 * terminal, -1 if no terminal is found. 
 */
static int guess_terminal() {
	for (int i = 0; supported_terminals[i][0] != NULL; i++) {
		if (terminal_exists(supported_terminals[i][0])) {
			return i; 
		}
	}
	return -1;
}

/* This function tries to spawn a new terminal and to execute the program
 * passed as an argument in the new terminal window as a child process.
 * It takes 2 arguments: the name of the program to run in a new terminal,
 * and a NULL-terminated list of the arguments taken by the program provided.
 * The argument list must NOT include the name of the program itself as the
 * first argument (unless needed for specific reasons); this will be
 * automatically managed by this function.
 * It returns the pid of the new generated process if everything was executer
 * successfully, -1 if the fork fails, -2 if no supported terminal was found,
 * and -3 if the terminal spawning failed.
 */
pid_t launch_in_new_terminal(const char *program_path, char *program_args[]) {
	int terminal_index = guess_terminal();

	if (terminal_index == -1) {
		fprintf(stderr, "Error: Could not find a suitable terminal emulator (xterm, gnome-terminal, etc.).\n");
		return -2;
	}

	pid_t pid = fork();
	if (pid < 0) {
		perror("Fork failed");
		return -1;
	}
	if (pid > 0) {
		return pid;
	}

	char** terminal = supported_terminals[terminal_index];

	int num_program_args = 0;
	while (program_args[num_program_args++]);

	printf("PID-1: %d\n", pid);
	int total_args = 3 + num_program_args;
	char *terminal_argv[total_args];

	terminal_argv[0] = terminal[0];
	terminal_argv[1] = terminal[1];
	terminal_argv[2] = (char *)program_path;
	for (int i = 0; i < num_program_args; i++) {
		terminal_argv[3 + i] = program_args[i];
	}
	terminal_argv[total_args - 1] = NULL;

	printf("PID-1: %d\n", pid);
	execvp(terminal_argv[0], terminal_argv);

	// This point is only reached if execvp failed
	perror("Execvp failed in child process");
	return -3;
}

