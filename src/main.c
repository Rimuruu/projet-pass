#include "errhandlers.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"

int main(int argc,char** argv){
    struct Game game;
    if(initGame(&game)){
        errmsgf("Init Game error");  
        return EXIT_FAILURE;
    }
    
    if(print_Game(&game)){
        errmsgf("print Game error");  
        return EXIT_FAILURE;
    }
    printf("sizeof game : %zu \n", sizeof(game));
    return EXIT_SUCCESS;
}