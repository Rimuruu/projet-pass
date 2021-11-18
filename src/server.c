#include "errhandlers.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "net.h"
#include <sys/select.h>
#include "enumvalue.h"


#define IP "127.0.0.1"
#define MAX 1024

struct Game game;
struct sockaddr_in serv_addr;
struct Client_info clients[2];
int nb_client = 0;
int listen_s;

uint16_t SERVERPORT = 7777;
fd_set listen_set, clients_set;

int max(int a, int b)
{
    return a > b ? a : b;
}

bool handle_connection()
{

    while (true)
    {
        if (nb_client < 2)
        {
            FD_SET(listen_s, &listen_set);
            select(listen_s + 1, &listen_set, NULL, NULL, NULL);
            if (FD_ISSET(listen_s, &listen_set))
            {
                if (accept_client(&(clients[nb_client].socket), &listen_s, (struct sockaddr *)&(clients[nb_client].addr), &(clients[nb_client].s_len)))
                {
                    errmsgf("err accept\n");
                }
                else
                {
                    clients[nb_client].f_w = fdopen(clients[nb_client].socket, "a+");
                    if(clients[nb_client].f_w== NULL){
                        return true;
                    }
                    clients[nb_client].f_r = fdopen(clients[nb_client].socket, "r");
                    if(clients[nb_client].f_r == NULL){
                        return true;
                    }
                    FD_SET(clients[nb_client].socket, &clients_set);
                    clients[nb_client].status = false;
                    debug_print("client connected\n");
                    nb_client++;
                }
            }
        }
        else
        {
            debug_print("server full\n");
            return false;
        }
    }
}


bool checking_status(){
    //int y,buff;
    //uint8_t  maxfd = max(clients[0].socket, clients[1].socket) + 1;
    while(!(clients[0].status) || !(clients[1].status)){
        
    }
    return false;
}


bool sending_status_check(){
    uint8_t buff[MAX],y;
    struct Info info;

    uint8_t  maxfd = max(clients[0].socket, clients[1].socket) + 1;
    for (y = 0; y < 2; y++)
        {
            while (true)
            {
                debug_print("sending to %d \n", y);
                FD_SET(clients[y].socket, &clients_set);
                if(select(maxfd, NULL, &clients_set, NULL, NULL) < 0){
                    return true;
                }
                if (FD_ISSET(clients[y].socket, &clients_set))
                {
                    size_t writeV;
                    info.type = PSTATUS;
                    
                    if(set_packet(buff,(uint8_t *) &info,sizeof(struct Info),INFO)){
                        break;
                    }
                  
                    writeV = fwrite(buff, sizeof(uint8_t), MAX,clients[y].f_w);
                    fflush(clients[y].f_w);
                    debug_print("write value %d feof %d, %d data write \n",ferror(clients[y].f_w),feof(clients[y].f_w),(int)writeV);
                  
                    break;
                }
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

    debug_print("DEBUG TEST\n");
    if (make_sockaddr(&serv_addr, IP, SERVERPORT))
    {
        errmsgf("err make sockaddr\n");
        return EXIT_FAILURE;
    }

    if (init_socket(&listen_s))
    {
        errmsgf("err init socket\n");
        return EXIT_FAILURE;
    }

    FD_ZERO(&listen_set);
    FD_ZERO(&clients_set);

    if (init_bind(&listen_s, (struct sockaddr *)&serv_addr))
    {
        errmsgf("err init bind\n");
        return EXIT_FAILURE;
    }

    if (init_listen(&listen_s))
    {
        errmsgf("err listen\n");
        return EXIT_FAILURE;
    }
    FD_SET(listen_s, &listen_set);
    


    if (handle_connection())
    {
        errmsgf("handle connection\n");
        return EXIT_FAILURE;
    }

    if (close_socket(&listen_s))
    {
        errmsgf("err close socket\n");
        return EXIT_FAILURE;
    }

    if (sending_status_check())
    {
        errmsgf("err waiting\n");
        return EXIT_FAILURE;
    }

    if(checking_status()){

        return EXIT_FAILURE;
    }

    if (close_socket(&clients[0].socket))
    {
        errmsgf("err close socket\n");
        return EXIT_FAILURE;
    }
    
    if (close_socket(&clients[1].socket))
    {
        errmsgf("err close socket\n");
        return EXIT_FAILURE;
    }
    
    fclose(clients[0].f_w);
    fclose(clients[0].f_r);
    fclose(clients[1].f_w);
    fclose(clients[1].f_r);

   
    return EXIT_SUCCESS;
}