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
            debug_print("Retrying\n");
            sleep(3);
        }
        else
        {
            FD_SET(listen_s, &server_set);
            debug_print("Connected\n");
            return false;
        }
    }
}

bool wait_for_start()
{
    char buff[MAX];
    int maxfd = max(STDIN_FILENO, listen_s) + 1;
    int readV;
    while (true)
    {
        FD_SET(STDIN_FILENO, &server_set);
        select(maxfd, &server_set, NULL, NULL, NULL);
        if (FD_ISSET(STDIN_FILENO, &server_set))
        {
            readV = read(STDIN_FILENO, buff, MAX);
            
            debug_print("DEBUG TEST");
            debug_print("test %d\n",readV);
            while (true)
            {
                FD_SET(listen_s, &server_set);
                select(maxfd, NULL, &server_set, NULL, NULL);
                if (FD_ISSET(listen_s, &server_set))
                {
                    debug_print("sending...\n");
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
            readV = read(listen_s, buff, sizeof(buff));
            debug_print("%d data read\n",readV);
            if (readV <1){
                errmsgf("Server disconnect\n");
                return true;
            }
            debug_print("Message from server: %s\t \n", buff);

            bzero(buff, MAX);
        }
    }
    return false;
}


bool waiting_status(){
    uint8_t buff,readV;
    uint8_t maxfd = max(STDIN_FILENO, listen_s) + 1;
    while(true){
        FD_SET(listen_s, &server_set);
        select(maxfd, &server_set, NULL, NULL, NULL);
        if (FD_ISSET(listen_s, &server_set))
        {
            readV = read(listen_s, &buff, sizeof(buff));
            debug_print("%d data read\n",readV);
            if (readV <1){
                errmsgf("Server disconnect\n");
                return true;
            }
            debug_print("Message from server: %d\t \n", buff);
            buff = 0;
            break;
        }

    }
    return false;


}

int main(int argc, char **argv)
{
 
    

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