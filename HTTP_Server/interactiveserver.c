#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <sys/stat.h>
#include <dirent.h>

#include "server.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);
    int server_socket = create_server_socket(port);
    printf("Server is running on port : %d\n", port);

    int client_socket = 0;
    while (1) {
        client_socket = accept(server_socket, NULL , NULL);
        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }
        handle_client(client_socket);
    }
    close(server_socket);
    return 0;
}

