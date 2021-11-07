#include "errhandlers.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "net.h"
#include <sys/select.h>
#include <unistd.h>
#define MAX 40

struct Game game;
struct sockaddr_in my_addr, serv_addr;
int listen_s;
fd_set server_set;

int max(int a, int b)
{
    return a > b ? a : b;
}

bool handle_connection()
{
    while (true)
    {
        if (connection_server(&listen_s, (struct sockaddr *)&serv_addr, sizeof(serv_addr)))
        {
            printf("Retrying\n");
            sleep(3);
        }
        else
        {
            FD_SET(listen_s, &server_set);
            printf("Connected\n");
            return false;
        }
    }
}

bool wait_for_start()
{
    char buff[MAX];
    int maxfd = max(STDIN_FILENO, listen_s) + 1;
    while (true)
    {
        FD_SET(STDIN_FILENO, &server_set);
        select(maxfd, &server_set, NULL, NULL, NULL);
        if (FD_ISSET(STDIN_FILENO, &server_set))
        {
            read(STDIN_FILENO, buff, MAX);
            while (true)
            {
                FD_SET(listen_s, &server_set);
                select(maxfd, NULL, &server_set, NULL, NULL);
                if (FD_ISSET(listen_s, &server_set))
                {
                    printf("sending...");
                    write(listen_s, buff, MAX);
                    break;
                }
            }
            bzero(buff, MAX);
        }
        FD_SET(listen_s, &server_set);
        select(maxfd, &server_set, NULL, NULL, NULL);
        if (FD_ISSET(listen_s, &server_set))
        {
            read(listen_s, buff, sizeof(buff));

            printf("From client: %s\t To client : \n", buff);

            bzero(buff, MAX);
        }
    }
    return false;
}

int main(int argc, char **argv)
{

    if (print_Game(&game))
    {
        errmsgf("print Game error\n");
        return EXIT_FAILURE;
    }

    if (initGame(&game))
    {
        errmsgf("Init Game error\n");
        return EXIT_FAILURE;
    }

    if (print_Game(&game))
    {
        errmsgf("print Game error\n");
        return EXIT_FAILURE;
    }
    if (make_sockaddr(&my_addr, "127.0.0.1", 0))
    {
        errmsgf("err make sockaddr\n");
        return EXIT_FAILURE;
    }

    if (make_sockaddr(&serv_addr, "127.0.0.1", 7777))
    {
        errmsgf("err make sockaddr\n");
        return EXIT_FAILURE;
    }

    if (init_socket(&listen_s))
    {
        errmsgf("err init socket\n");
        return EXIT_FAILURE;
    }

    if (init_bind(&listen_s, (struct sockaddr *)&my_addr))
    {
        errmsgf("err init bind\n");
        return EXIT_FAILURE;
    }
    printf("sizeof game : %zu \n", sizeof(game));
    FD_ZERO(&server_set);
    /*if(sendGame(&socket,&game, (struct sockaddr *) &serv_addr)){
        errmsgf("err send Game");
        return EXIT_FAILURE;
    }*/
    FD_SET(STDIN_FILENO, &server_set);
    if (handle_connection())
    {
        return EXIT_FAILURE;
    }

    if (wait_for_start())
    {
        return EXIT_FAILURE;
    }

    if (close_socket(&listen_s))
    {
        errmsgf("err close socket\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}