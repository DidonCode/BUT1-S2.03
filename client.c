#include "client.h"

void Error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int CreateSocket() {
    int socket_client_fd = 0;
    if ((socket_client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        Error("Socket failed");
    }
    return socket_client_fd;
}

void BindSocket(char *ip, struct sockaddr_in *serv_addr) {
    serv_addr->sin_family = AF_INET;
    serv_addr->sin_port = htons(PORT);

    if(inet_pton(AF_INET, ip, &serv_addr->sin_addr)<=0) {
        Error("Invalid IP address");
    }
}

void Connect(int socket_client_fd, struct sockaddr_in *serv_addr) {
    if (connect(socket_client_fd, (struct sockaddr *)serv_addr, sizeof(*serv_addr)) < 0) {
        Error("Connection failed");
    }
}

void SendMessage(int socket_client_fd, char *message) {
    if (send(socket_client_fd, message, strlen(message), 0) < 0) {
        Error("Send failed");
    }
}

void ReadResponse(int socket_client_fd, char* buffer) {
    int valread = read(socket_client_fd, buffer, 1023);
    if (valread < 0) {
        Error("Read failed");
    } else if (valread == 0) {
        printf("Connection closed by server\n");
        execute_client_state = 0;
    } else {
        buffer[valread] = '\0';
    }
}

void INThandler(int);


void INThandler(int sig){
    char  c;
    signal(sig, SIG_IGN);
    printf("\nVoulez vous vraiment quitter ? [y/n] ");
    c = getchar();
    if (c == 'y' || c == 'Y'){
        char message[10] = "disconnect";
        SendMessage(socket_client_fd, message);
        execute_client_state = 0;
        pthread_join(client_thread, NULL);
        close(socket_client_fd);
        exit(0);
    }    else{
        signal(SIGINT, INThandler);
    }
}


void *execute_client(void *argc) {
    int socket_client_fd = *(int*)argc;

    execute_client_state = 1;
    while(execute_client_state){
        char buffer[1024];
        ReadResponse(socket_client_fd, buffer);

        setCursorConsole(0, -1, buffer);
    }

    pthread_exit(NULL);
}