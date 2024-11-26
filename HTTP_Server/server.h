#ifndef SERVER_H_
#define SERVER_H_
#define BUFFER_SIZE 4096

int create_server_socket(int port);
void handle_client(int client_socket);
void path_decode(char *src, char *dest);
void get_the_content_of_path(const char *path, char **content);

#endif
