# Advanced Wait Function with Signal Handling

This project provides an advanced version of the `wait()` function for handling child process termination events. Instead of the traditional blocking wait, this implementation uses the `SIGCHLD` signal to notify the parent process about child status changes. It also allows for customized signal handling for child processes.

## Overview

In typical systems, the parent process uses the `wait()` or `waitpid()` system call to block and wait for its child processes to finish. However, this can cause inefficiencies if the parent is busy or needs to handle other tasks. This advanced implementation solves this by allowing the parent process to receive a signal (`SIGCHLD`) whenever a child terminates, enabling the parent to handle these events asynchronously and without blocking.

## Features

- **Non-blocking wait**: The parent process can continue with other tasks while waiting for child processes to finish.
- **Signal Handling**: The parent process is notified of child termination events via `SIGCHLD`.
- **Custom Handlers**: Allows for custom signal handlers to process child termination events.
- **Works with Interrupts**: The signal handler can be customized to handle interrupts during child process termination.

## Components

### `AdvWait.c`

Contains the core logic for handling child process termination via the `SIGCHLD` signal. It includes:
- A default signal handler (`handle_sigchld`) to process terminated child processes.
- The function `Activate_Interupted_Wait()` to enable the signal handler and allow the parent process to handle signals for terminated child processes.

### `AdvWait.h`

Header file that declares the function `Activate_Interupted_Wait()` to activate signal-based handling for child process termination.

### `test_wait.c`

A simple test application that creates multiple child processes and demonstrates the functionality of the signal handling system. It continuously waits for child events without blocking the parent process.

## How It Works

1. The parent process creates multiple child processes using `fork()`.
2. The parent process then activates the `SIGCHLD` signal handler using the `Activate_Interupted_Wait()` function.
3. Whenever a child process terminates, the operating system sends a `SIGCHLD` signal to the parent.
4. The parent handles the signal asynchronously, processing each child’s termination event without blocking.

## Functions

### `Activate_Interupted_Wait()`
```c
int Activate_Interupted_Wait(void (*handler)(int));
```

- **Purpose**: Activates the signal handler for `SIGCHLD` and enables the parent to handle child process termination asynchronously.
- **Parameters**:
  - `handler`: A pointer to a custom signal handler for `SIGCHLD`. If `NULL` is passed, the default handler is used.
- **Returns**:
  - `0` on success.
  - `-1` on failure, with an error message printed.

### `handle_sigchld()`
- **Purpose**: Default signal handler for `SIGCHLD` that handles child termination events by printing the child’s status (whether terminated normally or by a signal).

## Usage

1. **Include the Header File**: Include `AdvWait.h` in your application.
   
   ```c
   #include "AdvWait.h"
   ```

2. **Activate the Signal Handler**: In the parent process, call `Activate_Interupted_Wait()` to set up the signal handling.

   ```c
   Activate_Interupted_Wait(NULL);  // Use the default handler
   ```

3. **Handle Child Process Events**: Once the signal handler is activated, the parent process can continue to perform other tasks. When a child process terminates, the parent will receive the signal and handle the event.

4. **Compile and Run the Example**:
   Compile the test application:

    First, compile the AdvWait.c file:
    ```bash
    gcc -c AdvWait.c
    ```
    Next, create a static library:

    ```bash
    ar rcs libAdvWait.a AdvWait.o
    ```
    Then, compile the test application and link it with the library:

   ```bash
    gcc -o test_wait test_wait.c -L. -lAdvWait
   ```
    
   Run the program:
   ```bash
   ./test_wait
   ```
      The program will print messages about child process termination events in the parent process as they occur.
## Expected Output

```bash
Child process with PID 12345 running
Child process with PID 12346 running
Child process with PID 12347 running
Child with PID 12345 terminated by signal 9
Child with PID 12346 terminated normally with exit status 0
Child with PID 12347 terminated by signal 11
```

