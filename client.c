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

void SendMessage(int socket_client_fd, char* message) {
    if (send(socket_client_fd, message, strlen(message), 0) < 0) {
        Error("Send failed");
    }
    printf("Message envoyé\n");
}

void ReadResponse(int socket_client_fd, char* buffer) {
    int valread = read(socket_client_fd, buffer, 1023);
    if (valread < 0) {
        Error("Read failed");
    } else if (valread == 0) {
        printf("Connection closed by server\n");
    } else {
        buffer[valread] = '\0';
        printf("%s\n", buffer);
    }
}

int main(int argc, char const *argv[]) {
    int socket_client_fd;
    struct sockaddr_in serv_addr;
    char *message = "Hello from client";
    char buffer[1024];

    socket_client_fd = CreateSocket();
    BindSocket("127.0.0.1", &serv_addr);
    Connect(socket_client_fd, &serv_addr);

    SendMessage(socket_client_fd, message);
    ReadResponse(socket_client_fd, buffer);

    SendMessage(socket_client_fd, "d");
    close(socket_client_fd);
    return 0;
}