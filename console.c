#include "console.h"

int yConsole = 2;

void clear(){
    system("clear || cls");
}

void clearLigne(int y){
    if(y == 0){
        printf("\033[2K");
    }else{
        printf("\033[%dA\033[2K", y - 1);
    }
}

void setCursorConsole(int x, int y, char* message){
    if(y == -1){
        if(yConsole == 20){
            yConsole = 2;
            clear();
        } 
        printf("\033[%d;%dH%s", yConsole, x, message);
        yConsole++;
    }else{
        printf("\033[%d;%dH%s", y, x, message);
    }
    printf("\033[%d;%dH%s", 0, 10, "");
    fflush(stdout);
}