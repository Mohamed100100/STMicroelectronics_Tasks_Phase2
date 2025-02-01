# Logger Module

This repository contains a logger module implemented in C that supports dynamic log level control and the ability to turn on/off logging for different log types (ERROR, WARNING, INFO, DEBUG). It uses signals to modify log flags at runtime, allowing you to control the log output without restarting the application.

## Features

- **Log Types**: 
  - `ERROR`: Logs error messages in red.
  - `WARNING`: Logs warning messages in yellow.
  - `INFO`: Logs informational messages in green.
  - `DEBUG`: Logs debugging messages in cyan.

- **Dynamic Log Control**: 
  - Flags for each log type (ERROR, WARNING, INFO, DEBUG) can be turned ON/OFF at runtime using signals.
  - Logs are printed with colors corresponding to their severity level.
  
- **Signal Handling**: 
  - The module supports signal-based flag control using `SIGUSR1`.
  - The log flags can be changed dynamically while the program is running.

## File Structure

- `logger.c`: Contains the main logic for logging messages with dynamic flag control.
- `logger.h`: Contains function declarations, macros, and constants used in the logger module.
- `log_change.c`: A helper application that sends signals to change the log flags.
- `logger_test.c`: A test application to test the logger functionality.

## Usage

### 1. **Logger Module (`logger.c` and `logger.h`)**
   
   The `logger.c` file implements the core logging functionality, and the `logger.h` file provides the necessary declarations for different log types and functions.

   To use the logger module in your application, include `logger.h` and call :
   
   - `log_init()`: Enable the logger module.
   - `LOG_ERROR(format, ...)`: Logs an error message.
   - `LOG_WARNING(format, ...)`: Logs a warning message.
   - `LOG_INFO(format, ...)`: Logs an informational message.
   - `LOG_DEBUG(format, ...)`: Logs a debug message.

**Usage Example **:
   ```bash
LOG_ERROR("This is an error message with %d", 100);
LOG_WARNING("This is a warning message with %s", caution);
LOG_INFO("This is an informational message");
LOG_DEBUG("This is a debug message, useful for debugging");

   ```
### 2. **Log Flag Control (`log_change.c`)**

   The `log_change.c` application allows you to dynamically change the log flags by sending signals. It takes the process ID (PID) of the running application as an argument and sends signals to modify the log flags.

   **Usage**:
   ```bash
   gcc log_change.c -o log_change
   ./log_change <pid>
   ```

   After running this application, enter one of the following values to change the log flags:

   - `1`: Turn DEBUG flag ON
   - `2`: Turn DEBUG flag OFF
   - `3`: Turn INFO flag ON
   - `4`: Turn INFO flag OFF
   - `5`: Turn ERROR flag ON
   - `6`: Turn ERROR flag OFF
   - `7`: Turn WARNING flag ON
   - `8`: Turn WARNING flag OFF

### 3. **Testing the Logger (`logger_test.c`)**

   The `logger_test.c` application demonstrates how to use the logger. It continuously logs messages of various types and prints them based on the current log flag settings.

   **Usage**:
   ```bash
   gcc logger_test.c -o logger_test
   ./logger_test
   ```

   The application will repeatedly print logs based on the current flags. You can change the flags dynamically by using the `log_change` application.

## Example Output

```bash
Testing ERROR log:
This is an ERROR log message.

Testing WARNING log:
This is a WARNING log message.

Testing INFO log:
This is an INFO log message.

Testing DEBUG log:
This is a DEBUG log message.
```

### Changing Log Flags Dynamically

When running the `log_change` application, you can change the log flags in real-time. For example, turning off the ERROR flag will prevent the application from printing error messages.


## How to Compile

1. **Compile the Logger Module**:

   To compile the logger module, use the following command:
   ```bash
   # Step 1: Compile the logger.c source into an object file
    gcc -c logger.c
    ```
    ```bash
    # Step 2: Create the static library liblogger.a from the object file
    ar rcs liblogger.a logger.o
    ```
    ```bash
    # Step 3: Compile the test program and link with the static library
    gcc -o test_logger logger_test.c -L. -llogger

   ```

2. **Compile the Log Flag Control Application**:

   To compile the log flag control application:
   ```bash
   gcc log_change.c -o log_change
   ```


