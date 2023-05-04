#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "server.c"
//#include "client.c"

int main() {
    pthread_t server_thread; 
    
    while(1){
        char commande[1024];
        scanf("%s", commande);

        if(strncmp(commande, "start", 5) == 0){
            // Lancer le serveur dans un thread
            pthread_create(&server_thread, NULL, execute_server, NULL);   
        }
        else if(strncmp(commande, "stop", 5) == 0){
            pthread_cancel(server_thread);
            return 0;
        }
    
    }

    return 0;
}