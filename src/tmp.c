#include "terminal_launcher.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <string.h>

#define SHARED_MEMORY_SIZE 4096

// Function to initialize the shared memory
void *create_shared_memory() {
	void *shm = mmap(NULL, SHARED_MEMORY_SIZE, PROT_READ | PROT_WRITE,
			MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (shm == MAP_FAILED) {
		perror("mmap failed");
		exit(EXIT_FAILURE);
	}
	return shm;
}

int child_main(int argc, char *argv[], void *shared_memory) {
	printf("--- CHILD PROCESS EXECUTING IN NEW TERMINAL ---\n");
	printf("Child PID: %d\n", getpid());
	printf("Arguments received:\n");
	for (int i = 0; i < argc; i++) {
		printf("  argv[%d]: %s\n", i, argv[i]);
	}

	printf("Shared memory contains: %s\n", (char *)shared_memory);
	snprintf((char *)shared_memory, SHARED_MEMORY_SIZE, "Hello from child!");
	// snprintf((char *)shared_memory + 19, 50, "\nHello from child!");
	printf("Shared memory contains: %s\n", (char *)shared_memory);

	printf("Press Enter to close this terminal...\n");
	getchar();
	return 0;
}

int main(int argc, char *argv[]) {
	if (argc > 1 && strcmp(argv[1], "child") == 0)	return child_main(argc, argv, argv[2]);

	void *shared_memory = create_shared_memory();

	printf("Parent process: Shared memory contains: %s\n", (char *)shared_memory);
	snprintf((char *)shared_memory, SHARED_MEMORY_SIZE, "Hello from parent!");
	printf("Parent process: Shared memory contains: %s\n", (char *)shared_memory);


	char *child_args[] = { "child", shared_memory, "12345", NULL };

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

	printf("Parent process: Shared memory contains: %s\n", (char *)shared_memory);
	printf("Terminal process exited. Parent program closing.\n");


	return 0;
}

