#include "server.h"

int start_server(struct sockaddr_in address){
    // Creer le socket serveur
    int server_fd = create_server_socket();
    
    // Configurer l'adresse et le port du serveur
    bind_server_socket(server_fd, &address);
    
    // Mettre le serveur en écoute pour les connexions entrantes
    listen_server_socket(server_fd);

    return server_fd;
}

void stop_server(int server_fd){
    close(server_fd);

    printf("Server stopped.. OK \n");
}

int create_server_socket() {
    int server_fd;
    int opt = 1;
    
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        error("socket failed");
    }
    
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        error("setsockopt");
    }
    
    printf("Server started.. OK \n");

    return server_fd;
}

void bind_server_socket(int server_fd, struct sockaddr_in *address) {
    memset(address, 0, sizeof(*address));
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(PORT);
    
    if(bind(server_fd, (struct sockaddr *)address, sizeof(*address)) < 0) {
        error("bind failed");
    }
}

void listen_server_socket(int server_fd) {
    if(listen(server_fd, 3) < 0) {
        error("listen");
    }
}

int accept_client(int server_fd, struct sockaddr_in *address, socklen_t addrlen) {
    int client_fd;
    
    if((client_fd = accept(server_fd, (struct sockaddr *)address, &addrlen)) < 0) {
        error("accept");
    }
    
    return client_fd;
}

char* receive_client(int client_fd) {
    char *data = malloc(1024 * sizeof(char));

    // Lire le message envoyé par le client
    read(client_fd, data, 1024);
    printf("Client ID: %d, Receive Message: %s \n", client_fd, data);
    
    return data;
}

void send_client(int client_fd, char *data) {
    // Envoyer une information au client
    send(client_fd, data, strlen(data), 0);
    printf("Client ID: %d, Send Message: %s \n", client_fd, data);
}

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

//---------------\\

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

void *execute_server(void *arg){
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

    // Ferme le serveur
    stop_server(server_fd);

    pthread_exit(NULL);
}