#include "errhandlers.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "net.h"
#include <sys/select.h>

struct Game game;
struct sockaddr_in serv_addr;
struct Client_info clients[2];
int nb_client = 0;
int listen_s;
fd_set rfds;


bool handle_connection(){
    if (FD_ISSET(listen_s, &rfds)) {
            if(accept_client(&(clients[nb_client].socket), &listen_s,(struct sockaddr *) &(clients[nb_client].addr),&(clients[nb_client].s_len))){
                errmsgf("err accept\n");
                return true;


            }
            else{
                printf("client connected\n");
                nb_client++;
            
            }
    }
    return false;
}


int main(int argc,char** argv){
    

    
    
    if(make_sockaddr(&serv_addr,"127.0.0.1",7777)) {
        errmsgf("err make sockaddr\n");
        return EXIT_FAILURE;
    }

    if(init_socket(&listen_s)) {
        errmsgf("err init socket\n");
        return EXIT_FAILURE;
    }

    FD_ZERO(&rfds);
    

    
    if(init_bind(&listen_s,(struct sockaddr *) &serv_addr)) {
        errmsgf("err init bind\n");
        return EXIT_FAILURE;
    }

    if(init_listen(&listen_s)){
        errmsgf("err listen\n");
        return EXIT_FAILURE;
    }
    FD_SET(listen_s,&rfds);

    while(true){
    if(nb_client < 2 ){
        if(handle_connection()){
        errmsgf("err connection\n");
        return EXIT_FAILURE;
        }
    }
    else{
        printf("server full\n");
        return EXIT_SUCCESS;        
    }



    }

  
 
    
    /*if(recvGame(&socket,&game, (struct sockaddr *) &client_addr)){
        errmsgf("err rcv Game");
        return EXIT_FAILURE;
    }*/

   /* if(print_Game(&game)){
        errmsgf("print Game error");  
        return EXIT_FAILURE;
    }*/

    if(close_socket(&listen_s)){
        errmsgf("err close socket\n");  
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}