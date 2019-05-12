#ifndef SNAKE_FTP
#define SNAKE_FTP

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/stat.h>

void create_socket(void);

void get_response(int sock);

void send_request(int sock, const char* buf_request);

void send_file(int sock);

void create_socket_data(int sock, int port);

void filename(char *name);


#endif