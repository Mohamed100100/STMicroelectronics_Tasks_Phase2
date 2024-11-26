#ifndef PAGES_H_
#define PAGES_H_

#define BUFF_SIZE 4096

void send_page_1(int client_socket);
void send_page_2(int client_socket,const char *message) ;
void send_page_3(int client_socket, const char *message);
#endif
