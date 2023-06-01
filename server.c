#include "server.h"

int start_server(struct sockaddr_in address){
    int server_fd = create_server_socket();
    
    bind_server_socket(server_fd, &address);
    
    listen_server_socket(server_fd);

    return server_fd;
}

void stop_server(int server_fd){
    setCursorConsole(0, -1, "Server stopped.. OK");

    close(server_fd);
}

int create_server_socket() {
    int server_fd;
    int opt = 1;
    
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        error("ERROR on create socket");
    }
    
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        error("ERROR on setsockopt");
    }
    
    setCursorConsole(0, -1, "Server started.. OK");

    return server_fd;
}

void bind_server_socket(int server_fd, struct sockaddr_in *address) {
    memset(address, 0, sizeof(*address));
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(PORT);
    
    if(bind(server_fd, (struct sockaddr *)address, sizeof(*address)) < 0) {
        error("ERROR on bind server socket");
    }
}

void listen_server_socket(int server_fd) {
    if(listen(server_fd, 3) < 0) {
        error("ERROR on listening socket");
    }
}

int accept_client(int server_fd, struct sockaddr_in *address, socklen_t addrlen) {
    int client_fd;
    
    if((client_fd = accept(server_fd, (struct sockaddr *)address, &addrlen)) < 0) {
        error("ERROR on accept client");
    }
    
    return client_fd;
}

char* receive_client(int client_fd) {
    char *data = malloc(1024 * sizeof(char));

    if(read(client_fd, data, 1024) > 0){
        char message[1048];
        snprintf(message, sizeof(message), "Client ID: %d, Receive Message: %s", client_fd, data);
        setCursorConsole(0, -1, message);
    }
    return data;
}

void send_client(int client_fd, char *data) {
    send(client_fd, data, strlen(data), 0);

    char message[1048];
    snprintf(message, sizeof(message), "Client ID: %d, Send Message: %s", client_fd, data);
    setCursorConsole(0, -1, message);
}

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

//---------------\\

void send_all(char *data){
    for(int i = 0; i < MAX_CLIENT; i++){
        if(clients_fd != NULL && clients_fd[i] > 0){
            send_client(clients_fd[i], data);
        }
    }
}

int number_client(){
    int count = 0;
    for(int i = 0; i < MAX_CLIENT; i++){
        if (clients_thread != NULL && clients_fd != NULL && clients_fd[i] > 0 && pthread_tryjoin_np(clients_thread[i], NULL) != 0) {
            count++;
        }
    }

    return count;
}

void *handle_client(void *arg){
    int client_fd = *(int*)arg;

    while(1){
        char* data = receive_client(client_fd);

        if(strlen(data) == 10 && strncmp(data, "disconnect", 10) == 0){
            free(data);
            char text[1024] = "Cliend ID: ";
            char num[100];
            sprintf(num, "%d", client_fd);
            strcat(text, num);
            strcat(text, ", Disconnected !");
            setCursorConsole(0, -1, text);
            close(client_fd);
            pthread_exit(NULL);
        }else{
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
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);

    clients_thread = malloc(MAX_CLIENT * sizeof(pthread_t)); 
    clients_fd = malloc(MAX_CLIENT * sizeof(int)); 
    server_fd = malloc(1 * sizeof(int));

    for(int i = 0; i < MAX_CLIENT; i++){
        clients_thread[i] = 0;
    } 

    //-------------------\\

    *server_fd = start_server(address);

    execute_server_state = 1;
    while(execute_server_state){

        int client_fd = accept_client(*server_fd, &address, addrlen);

        if(client_fd > 0){
            for(int i = 0; i < MAX_CLIENT; i++){
                if(clients_thread[i] > 0 && pthread_tryjoin_np(clients_thread[i], NULL) == 0){
                    pthread_cancel(clients_thread[i]);
                    pthread_join(clients_thread[i], NULL);
                    clients_thread[i] = 0;
                    clients_fd[i] = 0;
                }
            }

            int index = 0;
            for(int i = 0; i < MAX_CLIENT; i++){
                if(clients_thread[i] == 0){
                    index = i;
                    break;
                }
            }

            char message[1024];
            snprintf(message, sizeof(message), "Client ID: %d, Connected !", client_fd);
            setCursorConsole(0, -1, message);

            clients_fd[index] = client_fd;
            pthread_create(&clients_thread[index], NULL, handle_client, &client_fd);
        }
    }

    pthread_exit(NULL);
}