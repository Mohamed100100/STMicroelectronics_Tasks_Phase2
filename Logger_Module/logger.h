// Signal used for log flag control
#define LOG_SIGNAL  (SIGUSR1)

#define DEBUG_ON    (1)
#define DEBUG_OFF   (2)
#define INFO_ON     (3)
#define INFO_OFF    (4)
#define ERR_ON      (5)
#define ERR_OFF     (6)
#define WARN_ON     (7)
#define WARN_OFF    (8)

// Color definitions for different log levels
#define COLOR_ERROR "\033[1;31m"  // Bold Red
#define COLOR_WARNING "\033[1;33m"  // Bold Yellow
#define COLOR_INFO "\033[1;32m"  // Bold Green
#define COLOR_DEBUG "\033[1;36m"  // Bold Cyan
#define COLOR_RESET "\033[0m"	

// Define the status of the log flag
typedef enum{
    ON, 	//Enable log output
    OFF 	// Disable log output
}flag_t;

// Log types for different severity levels
typedef enum {
    ERROR,   	// Error messages
    WARNING, 	// Warning messages
    INFO,    	// Informational messages
    DEBUG    	// Debugging messages
} log_type_t;

// Macros to simplify logging calls for different log types

// LOG_ERROR: Logs an error message with the specified format and arguments
// Equivalent to calling log_print(ERROR, format, ...)
#define LOG_ERROR(format, ...)   log_print(ERROR, format, ##__VA_ARGS__)

// LOG_WARNING: Logs a warning message with the specified format and arguments
// Equivalent to calling log_print(WARNING, format, ...)
#define LOG_WARNING(format, ...) log_print(WARNING, format, ##__VA_ARGS__)

// LOG_INFO: Logs an informational message with the specified format and arguments
// Equivalent to calling log_print(INFO, format, ...)
#define LOG_INFO(format, ...)    log_print(INFO, format, ##__VA_ARGS__)

// LOG_DEBUG: Logs a debug message with the specified format and arguments
// Equivalent to calling log_print(DEBUG, format, ...)
#define LOG_DEBUG(format, ...)   log_print(DEBUG, format, ##__VA_ARGS__)

void log_print(log_type_t log_type,const char *format, ...);
void log_init();

