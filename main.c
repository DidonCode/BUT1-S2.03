#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "console.c"
#include "server.c"
#include "client.c"


int main() {
    printf("Choisissez un mode ? \n");
    printf("1: Client ? \n");
    printf("2: Serveur ? \n");

    int reponse = 0;
    scanf("%d", &reponse);

    if(reponse == 1){ 
        clear();

        socket_client_fd = CreateSocket();
        struct sockaddr_in serv_addr;

        char ip[1024];
        printf("Entrez l'ip du serveur distant: ");
        scanf("%s", ip);

        BindSocket("127.0.0.1", &serv_addr);

        char pseudo[100];
        printf("Saisissez votre pseudo: ");
        scanf("%s", pseudo);

        Connect(socket_client_fd, &serv_addr);

        pthread_create(&client_thread, NULL, execute_client, (void *)&socket_client_fd);

        clear();

        setCursorConsole(0, -1, "Bienvenue dans le salon de chat !");

        sleep(1);

        while(1){
            clearLigne(0);
            setCursorConsole(0, 0, "Message: ");
            char message[1000];
            fgets(message, 1000, stdin);

            if(strlen(message) <= 1) continue;

            char m[1024] = "Vous: ";
            strcat(m, message);
            setCursorConsole(0, -1, m);

            char me[1024] = "\0";
            strcat(me, pseudo);
            strcat(me, ": ");
            strcat(me, message);
            SendMessage(socket_client_fd, me);

            signal(SIGINT, INThandler);
        }
    }

    if(reponse == 2){
        clear();
        setCursorConsole(0, -1, "Server Initialized.. OK");
        //printf("Server Initialized.. OK");
        sleep(1);

        pthread_t server_thread = 0; 
        
        while(1){
            clearLigne(0);
            setCursorConsole(0, 0, "Command: ");
            //printf("Command: ");

            char commande[1000];
            fgets(commande, 1000, stdin);

            if(strlen(commande) <= 3) continue;

            if(strncmp(commande, "start", 5) == 0){
                if(server_thread == 0){
                    pthread_create(&server_thread, NULL, execute_server, NULL);
                    sleep(1);
                }else{
                    setCursorConsole(0, -1, "ERROR start is not possible because the server already running !");
                }
            }
            else if(strncmp(commande, "stop", 4) == 0){
                if(server_thread != 0){
                    stop_server(*server_fd);

                    free(clients_thread);
                    free(clients_fd);
                    free(server_fd);

                    execute_server_state = 0;
                    pthread_cancel(server_thread);
                    pthread_join(server_thread, NULL);
                    
                    server_thread = 0;

                    sleep(1);
                }else{
                    setCursorConsole(0, -1, "ERROR stop is not possible because the server is not running !");
                }
            }
            else if(strncmp(commande, "quit", 4) == 0){
                if(server_thread == 0){
                    clear();
                    sleep(1);
                    return 0;
                }else{
                    setCursorConsole(0, -1, "ERROR quit is not possible because the server is running !");
                }
            }
            else if(strncmp(commande, "message ", 8) == 0){
                if(server_thread != 0){
                    char message[strlen(commande) - 8];

                    int i;
                    for (i = 0; i < strlen(commande) - 8; i++) {
                        message[i] = commande[i + 8];
                    }
                    message[i] = '\0';

                    char text[1024] = "Serveur: ";
                    strcat(text, message);

                    send_all(text);

                    sleep(1);
                }else{
                    setCursorConsole(0, -1, "ERROR message is not possible because the server is not running !");
                }
            }else if(strncmp(commande, "users", 5) == 0){
                char num[100];
                char text[100] = "DEBUG number of client: ";
                sprintf(num, "%d", number_client());
                strcat(text, num);
                setCursorConsole(0, -1, text);
            }
            else{
                setCursorConsole(0, -1, "ERROR this command does not exist !");
            }
    
        }
    }
    

    clear();
    return 0;
}