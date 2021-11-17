#include "errhandlers.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "net.h"
#include <sys/select.h>
#include "enumvalue.h"

#define SERVERPORT 7777
#define IP "127.0.0.1"
#define MAX 40

struct Game game;
struct sockaddr_in serv_addr;
struct Client_info clients[2];
int nb_client = 0;
int listen_s;
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

bool wait_for_start()
{
    int i, y, r;
    char buff[MAX];
    int maxfd = max(clients[0].socket, clients[1].socket) + 1;
    while (true)
    {
        for (i = 0; i < 2; i++)
        {
            debug_print("check client %d \n", i);
            FD_SET(clients[i].socket, &clients_set);
            select(maxfd, &clients_set, NULL, NULL, NULL);
            if (FD_ISSET(clients[i].socket, &clients_set))
            {
                debug_print("set %d\n", i);
                bzero(buff, MAX);
                // read the message from client and copy it in buffer
                r = read(clients[i].socket, buff, sizeof(buff));
                if(r < 1){
                    errmsgf("Client %d disconnect\n",i);
                    return true;
                }
                debug_print("From client %d: %s \n", buff, i);
                // print buffer which contains the client contents
                debug_print("end \n");
                for (y = 0; y < 2; y++)
                    {
                        while (true)
                        {
                            debug_print("sending to %d \n", y);
                            FD_SET(clients[y].socket, &clients_set);
                            select(maxfd, NULL, &clients_set, NULL, NULL);
                            if (FD_ISSET(clients[y].socket, &clients_set))
                            {
                
                                write(clients[y].socket, buff, sizeof(buff));
                                break;
                            }
                        }
                    }
                

                bzero(buff, MAX);
            }
        }
    }
    return false;
}

bool checking_status(){
    while(!(clients[0].status) || !(clients[0].status));
    return false;
}


bool sending_status_check(){
    uint8_t buff,y;
    uint8_t  maxfd = max(clients[0].socket, clients[1].socket) + 1;
    for (y = 0; y < 2; y++)
        {
            while (true)
            {
                debug_print("sending to %d \n", y);
                FD_SET(clients[y].socket, &clients_set);
                select(maxfd, NULL, &clients_set, NULL, NULL);
                if (FD_ISSET(clients[y].socket, &clients_set))
                {
                    buff = PSTATUS;
                    write(clients[y].socket, &buff, sizeof(buff));
                    break;
                }
            }
        }
    return false;
}

int main(int argc, char **argv)
{
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

    if (wait_for_start())
    {
        errmsgf("err waiting\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}