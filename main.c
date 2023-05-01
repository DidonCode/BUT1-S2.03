#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "server.c"
//#include "client.c"

const int MAX_CLIENT = 20;

int *clients_fd = NULL; 
pthread_t *clients_thread = NULL;


void *handle_client(void *arg){
    int client_fd = *(int*)arg;

    while(1){
        char* data = receive_client(client_fd);

        if(data[0] == 'c'){

        }

        if(data[0] == 'd'){
            close(client_fd);
            pthread_exit(NULL);
        }

        if(data[0] == 'm'){
            for(int i = 0; i < MAX_CLIENT; i++){
                if(clients_fd[i] > 0 && clients_fd[i] != client_fd){
                    send_client(clients_fd[i], data);
                }
            }
        }

        free(data);
    }
}

int main(){
    int server_fd;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);

    clients_thread = malloc(MAX_CLIENT * sizeof(pthread_t)); 
    clients_fd = malloc(MAX_CLIENT * sizeof(int)); 

    //-------------------\\

    // Demarer le serveur
    server_fd = start_server(address);

    while(1){

        // Accepter une connexion entrante
        int client_fd = accept_client(server_fd, &address, addrlen);

        if(client_fd > 0){
            printf("Cliend ID: %d, Connected \n", client_fd);

            int index = 0;
            for(int i = 0; i < MAX_CLIENT; i++){
                if(clients_thread[i] == 0){
                    index = i;
                    break;
                }
            }
            clients_fd[index] = client_fd;
            pthread_create(&clients_thread[index], NULL, handle_client, &client_fd);
        }
    }

    free(clients_thread);
    free(clients_fd);
    
    // Fermer la connexion
    stop_server(server_fd);

    return 0;
}