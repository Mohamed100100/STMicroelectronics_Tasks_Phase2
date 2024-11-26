#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <sys/stat.h>
#include <dirent.h>

#include "server.h"
#include "pages.h"

/**
 * @brief Create a server socket and start listening for client connections.
 *
 * This function initializes a server socket by creating, binding, and setting it to listen mode.
 * It abstracts the process of setting up a TCP server, allowing the user to specify the desired port.
 *
 * @param port : The port number on which the server should listen for incoming connections.
 * @return : Returns the server socket descriptor if successful.
 *           Exits the program with an error message if any step fails.
 */
int create_server_socket(int port) {
    /* server_socket to hold the server's socket descriptor*/
    int server_socket; 
    /* Structure to store server addresses*/
    struct sockaddr_in server_addr; 
   // socklen_t client_addr_len = sizeof(client_addr); // Size of the `client_addr` structure.

    /* Step 1       : Create a socket
     * AF_INET 	    : Address family for IPv4
     * SOCK_STREAM  : Socket type for TCP 
     * 0            : Use the default protocol 
     */
    server_socket = socket(AF_INET, SOCK_STREAM, 0); 

    if (server_socket < 0) {
        // If socket creation fails, print an error message and terminate the program.
    	perror("Socket creation failed");
        exit(1);
    }

    // Step 2: Configure server address
    /* Set the address family to IPv4 */
    /* Bind the socket to all available network interfaces on the machine */
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    /* Convert the port number to network byte order (big-endian) */
    server_addr.sin_port = htons(port); 

    /* Step 3: Bind the socket to the specified IP address and port */
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        
        // If binding fails, print an error message, close the socket, and terminate the program.
        perror("Bind failed");
        close(server_socket); // Release resources associated with the socket.
        exit(1);
    }

    /* Step 4 : Listen for incoming connections
     * listen : puts the server socket into passive mode, waiting for client connections.
     * The second argument (5) :  specifies the maximum number of pending connections in the queue.
    **/    
    if (listen(server_socket, 5) < 0) {
        perror("Listen failed");
        close(server_socket); 
        exit(1);
    }

    // If all steps succeed, return the server socket descriptor.
    return (server_socket);
}


/**
 * @brief Decode a URL-encoded string.
 * 
 * I Found that the received path has a problem (/usr >> %2Fusr) 
 * so the "/" is converted to "%2F" 
 * AND TO BE HONST , I ASKED FROM CHATGPT TO SOLVE GENERATE THIS FUNCTION :) 
 *
 * @param src : The URL-encoded input path to be decoded.
 * @param dest : The buffer where the decoded output path will be stored. 
 *               Ensure it has enough space to store the decoded result.
 * @return : No return value; the decoded string is stored in `dest`.
 */
void path_decode(char *src, char *dest) {
    char a, b; // Variables to hold the hexadecimal characters.

    // Loop through the source string until the null terminator.
    while (*src) {
        // If the current character is '%', check the next two characters.
        if ((*src == '%') &&
            ((a = src[1]) && (b = src[2])) && // Get the next two characters after '%'.
            (isxdigit(a) && isxdigit(b))) { // Check if both characters are valid hexadecimal digits.
            
            // Convert the first hexadecimal digit to uppercase if it's a lowercase letter.
            if (a >= 'a') a -= 'a' - 'A';
            // Convert the hexadecimal digit to its numeric value.
            if (a >= 'A') a -= ('A' - 10); // If it's a letter (A-F), map to 10-15.
            else a -= '0'; // If it's a digit (0-9), subtract '0' to get its numeric value.

            // Repeat the same process for the second hexadecimal digit.
            if (b >= 'a') b -= 'a' - 'A';
            if (b >= 'A') b -= ('A' - 10);
            else b -= '0';

            // Combine the two hexadecimal digits into a single byte and store it in `dest`.
            *dest++ = 16 * a + b;
            src += 3; // Move past the '%xx' sequence in `src`.
        } 
        // If the current character is '+', decode it as a space.
        else if (*src == '+') {
            *dest++ = ' '; // Replace '+' with a space in `dest`.
            src++; // Move to the next character in `src`.
        } 
        // For all other characters, copy them directly to `dest`.
        else {
            *dest++ = *src++;
        }
    }

    // Null-terminate the destination string.
    *dest = '\0';
}


/**
 * @brief Retrieve the content of a specified file path.
 *
 * This function processes a given file path and determines its type (directory, regular file, or executable file). 
 * It retrieves and stores the content of the path in a dynamically allocated buffer (`content`).
 * If the path is a directory, it lists its contents. 
 * If it is a regular file, it reads its contents.
 * If it is an executable file (CGI script), it executes it and captures the output.
 * In case of an error, an appropriate error message is stored in `content`.
 *
 * @param path : The file system path to process (directory, file, or CGI script).
 * @param content : Pointer to a buffer where the retrieved content or error message will be stored.
 *                  The caller must free the allocated memory.
 * @return : No return value; the output is stored in the dynamically allocated `content` buffer.
 */

void get_the_content_of_path(const char *path, char **content) {
    struct stat path_stat;
   
    /* --------------------------- ERROR PART --------------------------- */
    /* Check if the path exists */
    if (stat(path, &path_stat) != 0) {
        /* Allocate memory for the error message */
        *content = malloc(BUFFER_SIZE);
        memset(*content,'\0',BUFFER_SIZE); 
        snprintf(*content, BUFFER_SIZE, "Error 404 Not Found <br> Resource not found: %s", path);
        printf("The Path is not found \n");
        return;
    }

    /* --------------------------- DIRECTORY PART --------------------------- */
    if (S_ISDIR(path_stat.st_mode)) {
    /* Open the directory */
        DIR *dir = opendir(path); 
       
        if (!dir) {
           /* Log error if directory cannot be opened */
            perror("opendir"); 
            *content = strdup("Failed to open directory.");
             printf("Failed to open directory\n");
            return;
        }

        /* Directory opened successfully */
        printf("Upload The Directory content .......\n");
        /* Allocate memory for directory content */
        *content = malloc(BUFFER_SIZE); 
	memset(*content,'\0',BUFFER_SIZE);
	*content = strcpy(*content ,"The Directory Content : <br> <br>");
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            strncat(*content, entry->d_name, BUFFER_SIZE - strlen(*content) - 1);
            strncat(*content, "<br>", BUFFER_SIZE - strlen(*content) - 1);
        }
        closedir(dir); 
        printf("Done\n");

    /* -------------------------- REGULAR FILES PART ------------------------- */
    } else if (S_ISREG(path_stat.st_mode)) {
    	/* Check if the file is not executable */
        if (!(path_stat.st_mode & S_IXUSR)) {
            /* Open the file for reading */
            FILE *file = fopen(path, "r"); 
            if (!file) {
            /* Log error if file cannot be opened */
                perror("fopen"); 
                *content = strdup("Failed to open file.");
                return;
            }
            printf("Upload The File content .......\n");
            /* Allocate memory for file content */
            *content = malloc(BUFFER_SIZE); 
            memset(*content,'\0',BUFFER_SIZE);
            *content = strcpy(*content ,"The File Content : <br> <br>");
            char buffer[BUFFER_SIZE] = {'\0'};
            size_t bytes_read;
            // Read the file content into the buffer
            while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
                strncat(*content, buffer, BUFFER_SIZE - strlen(*content) - 1);
                // Clear the buffer after use
                memset(buffer, '\0', bytes_read); 
            }
          fclose(file); 
            printf("Done\n");

        /* --------------------------- CGI FILES PART --------------------------- */
        } else { 
            /* Handle executable files (CGI scripts)
             * Execute the CGI script and open a pipe
             **/
           FILE *file = popen(path, "r"); 
            if (!file) {
                perror("popen"); // Log error if execution fails
                *content = strdup("Failed to execute CGI script.");
                return;
            }
            printf("Execute The CGI File .......\n");
            /* Allocate memory for CGI output */
            *content = malloc(BUFFER_SIZE); 
	     memset(*content,'\0',BUFFER_SIZE);
	    *content = strcpy(*content ,"The OUTPUT is : <br> <br>");
            char buffer[BUFFER_SIZE] = {'\0'};
            // Read the output of the CGI script
            while (fgets(buffer, sizeof(buffer), file)) {
                strncat(*content, buffer, BUFFER_SIZE - strlen(*content) - 1);
                memset(buffer, '\0', BUFFER_SIZE); // Clear the buffer after use
            }
            pclose(file); 
            printf("Done\n");
        }
    }
}

/**
 * @brief Handle client requests and provide appropriate responses.
 *
 * This function processes incoming requests from a connected client, determines the type of request (GET or POST), 
 * and takes the necessary action. It supports rendering pages, handling user input, decoding paths, retrieving file 
 * or directory contents, and sending appropriate responses back to the client.
 *
 * @param client_socket : The socket descriptor for the connected client.
 * @return : No return value; the function manages communication and response generation for the client.
 */

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE] = {'\0'};
    // Read the client's request into the buffer
    int read_size = read(client_socket, buffer, BUFFER_SIZE - 1);
    if (read_size < 0) {
        perror("Failed to read from client"); 
        close(client_socket); 
        return;
    }
    /* Null-terminate the buffer to ensure it is a valid string */
    buffer[read_size] = '\0'; 

    /* Handle a GET request to the root path
     * Send the first page to the client
     **/    
    if (strncmp(buffer, "GET / ", 6) == 0) {
        send_page_1(client_socket); 
    }
    // Handle a POST request to the root path
    else if (strncmp(buffer, "POST / ", 7) == 0) {
        // Extract the "name" parameter
        char *name = strstr(buffer, "name=") + 5; 
        // Separate the name value
        name = strtok(name, "&"); 
        // Log the connected user's name
        printf("User connected: %s\n", name); 
        // Send the second page to the client
        send_page_2(client_socket,name); 
    }
    // Handle a POST request to the "/send" path
    else if (strncmp(buffer, "POST /send ", 11) == 0) {
        // Extract the "message" parameter
        char *message = strstr(buffer, "message=") + 8; 
        // Separate the path value
        char *path = strtok(message, "&"); 

	// Decode the URL-encoded path
        char decoded_path[1000] = {'\0'};
        path_decode(path, decoded_path); 
        char *content = NULL;
        // Log the received path
        printf("Received path: %s\n", decoded_path); 
        // Retrieve the content of the path
        get_the_content_of_path(decoded_path, &content); 
        // Send the retrieved content to the client
        send_page_3(client_socket, content); 

        memset(content, '\0', sizeof(content)); 
        free(content); 
        content = NULL;
    }
    // Handle a POST request to the "/back" path
    // Send the second page to the client
    else if (strncmp(buffer, "POST /back ", 11) == 0) {
        send_page_2(client_socket," "); 
    }
    // Handle a POST request to the "/end" path
    // Send the first page to the client
    else if (strncmp(buffer, "POST /end ", 10) == 0) {
        send_page_1(client_socket); 
        // Log the user's disconnection
        printf("User Disconnected\n"); 
        close(client_socket); 
    }
    
}


