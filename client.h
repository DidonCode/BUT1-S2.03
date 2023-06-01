#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "signal.h"

#define PORT 8080

int socket_client_fd;
pthread_t client_thread = 0;
int execute_client_state = 1;

int CreateSocket();
void BindSocket(char *ip, struct sockaddr_in *serv_addr);
void Connect(int socket_client_fd, struct sockaddr_in *serv_addr);

void SendMessage(int socket_client_fd, char message[1000]);
void ReadResponse(int socket_client_fd, char *buffer);

void INThandler(int);
void  INThandler(int sig);

void *execute_client(void *argc);
#endif