#include "errhandlers.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "net.h"
#include <sys/select.h>

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
                    printf("client connected\n");
                    nb_client++;
                }
            }
        }
        else
        {
            printf("server full\n");
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
            printf("check client %d", i);
            FD_SET(clients[0].socket, &clients_set);
            FD_SET(clients[1].socket, &clients_set);
            select(maxfd, &clients_set, NULL, NULL, NULL);
            if (FD_ISSET(clients[i].socket, &clients_set))
            {
                printf("set %d", i);
                bzero(buff, MAX);
                // read the message from client and copy it in buffer
                r = read(clients[i].socket, buff, sizeof(buff));
                printf("From client: %s %d\n", buff, r);
                // print buffer which contains the client contents
                printf("end \n");

                for (y = 0; y < 2; y++)
                {
                    while (true)
                    {
                        printf("sendin to %d \n", y);
                        FD_SET(clients[0].socket, &clients_set);
                        FD_SET(clients[1].socket, &clients_set);
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

int main(int argc, char **argv)
{

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

    /*if(recvGame(&socket,&game, (struct sockaddr *) &client_addr)){
        errmsgf("err rcv Game");
        return EXIT_FAILURE;
    }*/

    /* if(print_Game(&game)){
        errmsgf("print Game error");  
        return EXIT_FAILURE;
    }*/

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

    if (wait_for_start())
    {
        errmsgf("err waiting\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}