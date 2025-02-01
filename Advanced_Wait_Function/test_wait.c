#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "AdvWait.h"

int main() {
    // Create multiple child processes for demonstration
    for (int i = 0; i < 3; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }
        if (pid == 0) {
            // Child process
            printf("Child process with PID %d running\n", getpid());
	    while(1); 
        }
    }

    sleep(1);
    if((Activate_Interupted_Wait(NULL)) == -1){
       printf("Error with \"Activate_Interupted_Wait\" function \n");
    }
    printf("Parent process waiting for child events...\n");

    // Parent process continues its work without blocking
    while (1) {
    }
    return 0;
}

