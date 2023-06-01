#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define PORT 8080
#define MAX_CLIENT 20

int *clients_fd = NULL; 
pthread_t *clients_thread = NULL;
int *server_fd = NULL;
int execute_server_state = 1;

int start_server();
void stop_server();

int create_server_socket();
void bind_server_socket(int server_fd, struct sockaddr_in *address);
void listen_server_socket(int server_fd);

int accept_client(int server_fd, struct sockaddr_in *address, socklen_t addrlen);
char* receive_client(int client_fd);
void send_client(int client_fd, char* data);
int number_client();

void error(const char *msg);

void *handle_client(void *arg);
void *execute_server(void *arg);



#endif