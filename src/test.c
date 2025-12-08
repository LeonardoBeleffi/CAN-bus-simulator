#include "terminal_launcher.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int child_main(int argc, char *argv[]) {
	printf("--- CHILD PROCESS EXECUTING IN NEW TERMINAL ---\n");
	printf("Child PID: %d\n", getpid());
	printf("Arguments received:\n");
	for (int i = 0; i < argc; i++) {
		printf("  argv[%d]: %s\n", i, argv[i]);
	}
	printf("Press Enter to close this terminal...\n");
	getchar();
	return 0;
}


int main(int argc, char *argv[]) {
	if (argc > 1 && strcmp(argv[1], "child") == 0)	return child_main(argc, argv);

	printf("--- PARENT PROCESS STARTING ---\n");
	printf("Parent PID: %d\n", getpid());

	char *child_args[] = { "child", "Message",  "12345", NULL };

	pid_t new_terminal_pid = launch_in_new_terminal(argv[0], child_args);

	if (new_terminal_pid == -1) {
		fprintf(stderr, "Fatal error: Fork failed.\n");
		return EXIT_FAILURE;
	} else if (new_terminal_pid == -2) {
		fprintf(stderr, "Fatal error: No supported terminal was found.\n");
		return EXIT_FAILURE;
	} else if (new_terminal_pid == -3) {
		fprintf(stderr, "Fatal error: Unable to spawn the terminal.\n");
		return EXIT_FAILURE;
	}

	printf("New terminal launched successfully (PID of terminal process: %d)\n", new_terminal_pid);

	int status;
	printf("Parent waiting for the new terminal...\n");
	waitpid(new_terminal_pid, &status, 0); 

	printf("Terminal process exited. Parent program closing.\n");

	return 0;
}

