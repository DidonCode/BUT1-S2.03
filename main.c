#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "console.c"
#include "server.c"
//#include "client.c"

int main() {
    printf("Choisissez un mode ? \n");
    printf("1: Client ? \n");
    printf("2: Serveur ? \n");

    int reponse = 0;
    scanf("%d", &reponse);

    if(reponse == 1){
        
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
            else if(strncmp(commande, "message", 7) == 0){
                if(server_thread != 0){
                    char message[1048];
                    char *line;
                    char *result = strstr(commande, "message ");
                    strcpy(line, result + strlen("message "));

                    snprintf(message, sizeof(message), "m%s", line);
                    send_all(message);

                    sleep(1);
                }else{
                    setCursorConsole(0, -1, "ERROR message is not possible because the server is not running !");
                }
            }else{
                setCursorConsole(0, -1, "ERROR this command does not exist !");
            }
			
			printf("%d", number_client());
    
        }
    }

    clear();
    return 0;
}