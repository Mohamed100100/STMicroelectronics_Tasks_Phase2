#include <stdlib.h>     
#include <unistd.h>    
#include <signal.h>  
#include <stdio.h>
#include <sys/wait.h>

/**
 * @brief Default signal handler for SIGCHLD signal.
 *        This handler processes child termination events.
 */
void handle_sigchld() {
    int wstatus;
    pid_t pid;

    // Non-blocking wait for all terminated child processes
    // WNOHANG ensures that waitpid() doesn't block if no child has exited
    while ((pid = waitpid(-1, &wstatus, WNOHANG)) > 0) {
        if (WIFEXITED(wstatus)) {
            // Child terminated normally
            printf("Child with PID %d terminated normally with exit status %d\n",
                   pid, WEXITSTATUS(wstatus));
        } else if (WIFSIGNALED(wstatus)) {
            // Child terminated by a signal
            printf("Child with PID %d terminated by signal %d\n",
                   pid, WTERMSIG(wstatus));
        }
    }
}

/**
 * @brief Activates signal handling for interrupted child processes.
 *
 * @param handler A pointer to a custom signal handler function for SIGCHLD.
 *                If NULL, the default handler `handle_sigchld` is used.
 * @return 0 on success, -1 on failure (with error printed).
 */
int Activate_Interupted_Wait(void (*handler)(int)) {
    struct sigaction signal_options;

    // Select the handler: use the default if the provided handler is NULL
    if (handler == NULL) {
        signal_options.sa_handler = handle_sigchld;
    } else {
        signal_options.sa_handler = handler;
    }

    // Flags to configure the behavior of the signal handler
    // SA_RESTART: Automatically restart certain system calls if interrupted (ex : read)
    // SA_NOCLDSTOP: Ignore stopped child processes, only handle terminated ones
    signal_options.sa_flags = SA_RESTART | SA_NOCLDSTOP;

    // Install the signal handler for SIGCHLD
    if ((sigaction(SIGCHLD, &signal_options, NULL)) == -1) {
        perror("failed");  
        return -1;         
    }

    return 0;  
}

