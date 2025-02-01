#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>
#include <signal.h>

#include "logger.h"

// Global flags to enable or disable logs for each log type
flag_t err_flag   = ON;
flag_t debug_flag = ON;
flag_t warn_flag  = ON;
flag_t info_flag  = ON;


// Function to block the LOG_SIGNAL during critical log operations
static void disable_signal(){
    sigset_t signal_set;
    // Block SIGUSR1 to queue it as pending
    sigemptyset(&signal_set);
    sigaddset(&signal_set, LOG_SIGNAL);
    if (sigprocmask(SIG_BLOCK, &signal_set, NULL) == -1) {
        perror("Failed to block SIGUSR1");
        exit(EXIT_FAILURE);
    }
}

// Function to unblock the LOG_SIGNAL after critical log operations
static void enable_signal(){
    sigset_t signal_set;
    // UNBlock SIGUSR1 
    sigemptyset(&signal_set);
    sigaddset(&signal_set, LOG_SIGNAL);
    if (sigprocmask(SIG_UNBLOCK, &signal_set, NULL) == -1) {
	perror("Failed to unblock SIGUSR1");
        exit(EXIT_FAILURE);
    }
}

// Function to print log messages with color and variable arguments
void log_print(log_type_t log_type,const char *format, ...){
     // Flag to indicate if logging should proceed
     flag_t flag = OFF;  
     
     // Temporarily disable the log control signal
     disable_signal(); 
     switch(log_type){
	case(ERROR):
		if(OFF == err_flag)break;

		printf("%s", COLOR_ERROR);
		flag = ON;
		break;
	case(WARNING):
		if(OFF == warn_flag)break;
                
		printf("%s",COLOR_WARNING);
		flag = ON;
		break;
	case(DEBUG):
                if(OFF == debug_flag)break;
                
		printf("%s", COLOR_DEBUG);
                flag = ON;        
		break;
	case(INFO):
                if(OFF == info_flag)break;
                printf("%s", COLOR_INFO);
                flag = ON;                
		break;
     }
     // Re-enable the log control signal
     enable_signal();

     // Print the log if the flag is set
     if(flag == ON){
             va_list args;
	     va_start(args, format);
             vprintf(format, args);
             va_end(args);
     }
     printf("%s", COLOR_RESET);
}




// Signal handler to process log control signals and update log flags
void handle_signal(int sig, siginfo_t *info, void *context) {        
    // extract the value from the signal info 
    int value = info->si_value.sival_int;
    switch(value){
	    case(DEBUG_ON)  : debug_flag = ON ;break;
	    case(DEBUG_OFF) : debug_flag = OFF;break;
	    case(INFO_ON)   : info_flag  = ON ;break; 
	    case(INFO_OFF)  : info_flag  = OFF;break; 
	    case(ERR_ON)    : err_flag   = ON ;break;
	    case(ERR_OFF)   : err_flag   = OFF;break;
	    case(WARN_ON)   : warn_flag  = ON ;break;
	    case(WARN_OFF)  : warn_flag  = OFF;break;
    }
}

void log_init(){
     struct sigaction signal_options;
     // Set the custom handler
     signal_options.sa_sigaction = handle_signal;
     // Enable signal info delivery
     signal_options.sa_flags = SA_SIGINFO|SA_RESTART;
     
    // Register the signal handler for SIGUSR1
    if (sigaction(LOG_SIGNAL, &signal_options, NULL) == -1) {
        perror("Failed to set signal handler");
    }
}
