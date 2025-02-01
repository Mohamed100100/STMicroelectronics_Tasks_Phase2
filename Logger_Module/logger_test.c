#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

// Assuming the logger module is implemented in 'logger.h' and 'logger.c'

#include "logger.h"

// Function to simulate log printing for different log types
void test_log_print() {
    // Test log prints for each log type
    printf("Testing ERROR log:\n");
    LOG_ERROR("This is an ERROR log message.\n");

    printf("Testing WARNING log:\n");
    LOG_WARNING("This is a WARNING log message.\n");

    printf("Testing INFO log:\n");
    LOG_INFO("This is an INFO log message.\n");

    printf("Testing DEBUG log:\n");
    LOG_DEBUG("This is a DEBUG log message.\n");
}

int main() {
    pid_t pid = getpid(); // Get the current process ID
    printf("pid : %d\n ",pid);
    // Initialize the signal handler
    log_init();

    // Test log printing with default flags
    while(1){
    	test_log_print();
	getchar();	 
    }
    return 0;
}

