#include "errhandlers.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "net.h"
#include <sys/select.h>
#include <unistd.h>
#define MAX 1024

struct Game game;

struct Info infoG;

struct Server_info serv_info;

struct sockaddr_in my_addr, serv_addr;
uint16_t SERVERPORT = 7777;
fd_set server_set;

int max(int a, int b)
{
    return a > b ? a : b;
}

bool handle_connection()
{
    while (true)
    {
        if (connection_server(&(serv_info.socket), (struct sockaddr *)&serv_addr, sizeof(serv_addr)))
        {
            debug_print("Retrying\n");
            sleep(3);
        }
        else
        {
            serv_info.f_r = fdopen(serv_info.socket, "r");
            if(serv_info.f_r == NULL){
                return true;
            }
            serv_info.f_w = fdopen(serv_info.socket, "w");
            if(serv_info.f_w == NULL){
                return true;
            }
            FD_SET(serv_info.socket, &server_set);
            debug_print("Connected\n");
            return false;
        }
    }
}




bool waiting_status(){
    uint8_t buff[MAX];
    uint8_t maxfd = (uint8_t) max(STDIN_FILENO, serv_info.socket) + 1;
    while(true){
        FD_SET(serv_info.socket, &server_set);
        select(maxfd, &server_set, NULL, NULL, NULL);
        if (FD_ISSET(serv_info.socket, &server_set))
        {
            size_t readV;
            debug_print("recv \n");
            readV = fread(buff,sizeof(uint8_t),MAX,serv_info.f_r);
            debug_print("%d bytes data read\n",(int)readV);
            if(buff[0] == INFO){
                memcpy(&infoG, buff+1, sizeof(struct Info));
                debug_print("%d info\n",infoG.type);
            }
            
            debug_print("data read %d\n",buff[0]);
            if (readV <1){
                
                errmsgf("Server disconnect\n");
                return true;
            }
            debug_print("Message from server: %d\t \n", (int) buff[0]);
            
            
            break;
        }

    }
    return false;


}

int main(int argc, char **argv)
{
 
    if(argc != 2){
         errmsgf("arg err\n");
        return EXIT_FAILURE;
    }
    SERVERPORT = (uint16_t)atoi(argv[1]);

    if (initGame(&game))
    {
        errmsgf("Init Game error\n");
        return EXIT_FAILURE;
    }


    if (make_sockaddr(&my_addr, "127.0.0.1", 0))
    {
        errmsgf("err make sockaddr\n");
        return EXIT_FAILURE;
    }

    if (make_sockaddr(&serv_addr, "127.0.0.1", SERVERPORT))
    {
        errmsgf("err make sockaddr\n");
        return EXIT_FAILURE;
    }

    if (init_socket(&serv_info.socket))
    {
        errmsgf("err init socket\n");
        return EXIT_FAILURE;
    }

    if (init_bind(&serv_info.socket, (struct sockaddr *)&my_addr))
    {
        errmsgf("err init bind\n");
        return EXIT_FAILURE;
    }
    debug_print("sizeof game : %zu \n", sizeof(game));
    FD_ZERO(&server_set);
   
    FD_SET(STDIN_FILENO, &server_set);
    if (handle_connection())
    {
        return EXIT_FAILURE;
    }

    if (waiting_status())
    {
        return EXIT_FAILURE;
    }


  

    if (close_socket(&serv_info.socket))
    {
        errmsgf("err close socket\n");
        return EXIT_FAILURE;
    }
    fclose(serv_info.f_w);
    fclose(serv_info.f_r);

    return EXIT_SUCCESS;
}