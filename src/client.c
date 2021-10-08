#include "errhandlers.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "net.h"

int main(int argc,char** argv){
    struct Game game;
    struct sockaddr_in my_addr,serv_addr;
    int socket;

    if(print_Game(&game)){
        errmsgf("print Game error");  
        return EXIT_FAILURE;
    }

    if(initGame(&game)){
        errmsgf("Init Game error");  
        return EXIT_FAILURE;
    }
    
    if(print_Game(&game)){
        errmsgf("print Game error");  
        return EXIT_FAILURE;
    }
    if(make_sockaddr(&my_addr,"127.0.0.1",0)) {
        errmsgf("err make sockaddr");
        return EXIT_FAILURE;
    }

    
    if(make_sockaddr(&serv_addr,"127.0.0.1",7777)) {
        errmsgf("err make sockaddr");
        return EXIT_FAILURE;
    }

    if(init_socket(&socket)) {
        errmsgf("err init socket");
        return EXIT_FAILURE;
    }

    
    if(init_bind(&socket,(struct sockaddr *) &my_addr)) {
        errmsgf("err init bind");
        return EXIT_FAILURE;
    }
    printf("sizeof game : %zu \n", sizeof(game));
    
    if(sendGame(&socket,&game, (struct sockaddr *) &serv_addr)){
        errmsgf("err send Game");
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}