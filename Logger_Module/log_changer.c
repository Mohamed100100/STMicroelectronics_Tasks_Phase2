#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

// Signal used for log flag control
#define LOG_SIGNAL  (SIGUSR1)

// Function to send a signal to change the flags
void send_signal_to_change_flag(pid_t pid, int flag_value) {
    union sigval value;
    value.sival_int = flag_value;
    if (sigqueue(pid, LOG_SIGNAL, value) == -1) {
        perror("Failed to send signal");
        exit(EXIT_FAILURE);
    }
}


int main(int argc, char *argv[]) {   
    //handle the case of wrong input
    if (argc != 2) {
        printf("Usage: %s <pid> \n", argv[0]);        
	return 1;
    }
    // get the pid and the transmitted value
    pid_t pid = atoi(argv[1]);

    printf("Send signal to change flags:\n");
    printf("1: Turn DEBUG flag ON\n");
    printf("2: Turn DEBUG flag OFF\n");
    printf("3: Turn INFO flag ON\n");
    printf("4: Turn INFO flag OFF\n");
    printf("5: Turn ERR flag ON\n");
    printf("6: Turn ERR flag OFF\n");
    printf("7: Turn WARN flag ON\n");
    printf("8: Turn WARN flag OFF\n");

    int input;
    while (1) {
        printf("Enter a signal value (1-8) or -1 to exit: ");
        if (scanf("%d", &input) != 1 || input == -1) {
            break;
        }

        if (input < 1 || input > 8) {
            printf("Invalid value. Please enter a value between 1 and 8.\n");
            continue;
        }

        // Send signal to change the flag
        send_signal_to_change_flag(pid, input);
    }

    printf("Exiting application.\n");
    return 0;
}
