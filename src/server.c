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

uint32_t SERVERPORT = 7777;
fd_set listen_set, clients_set;

int max(int a, int b)
{
    return a > b ? a : b;
}

bool swap_player()
{
    struct Client_info tmp = clients[0];
    clients[0] = clients[1];
    clients[1] = tmp;
    return false;
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
                    clients[nb_client].f_w = fdopen(clients[nb_client].socket, "w+");
                    if (clients[nb_client].f_w == NULL)
                    {
                        return true;
                    }
                    clients[nb_client].f_r = fdopen(clients[nb_client].socket, "r+");
                    if (clients[nb_client].f_r == NULL)
                    {
                        return true;
                    }

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

bool sending_player_turn()
{
    uint8_t buff[MAX], y;
    uint8_t m[32];
    struct Word msg;

    for (y = 0; y < 2; y++)
    {
        if (sprintf((char *)m, "Your are player %d", y + 1) < 0)
        {
            return true;
        }
        if (initWord(&msg, m, 32))
        {
            return true;
        }
        if (set_packet(buff, (uint8_t *)&msg, sizeof(struct Word), MSG))
        {
            break;
        }
        send_packet(buff, clients[y].socket, &clients_set, clients[y].f_w);
    }
    return false;
}

bool send_score()
{
    uint8_t buff[MAX], y;
    uint8_t m[32];
    struct Word msg;

    for (y = 0; y < 2; y++)
    {
        if (sprintf((char *)m, "Your score %d", game.score) < 0)
        {
            return true;
        }
        if (initWord(&msg, m, 32))
        {
            return true;
        }
        if (set_packet(buff, (uint8_t *)&msg, sizeof(struct Word), MSG))
        {
            break;
        }
        if (send_packet(buff, clients[y].socket, &clients_set, clients[y].f_w))
        {
            return true;
        }
    }
    return false;
}

bool ask_maxword()
{
    uint8_t buff[MAX];
    if (set_packet(buff, NULL, 0, MAXWORD))
    {
        return true;
    }
    if (send_packet(buff, clients[0].socket, &clients_set, clients[0].f_w))
    {
        return true;
    }
    bzero(buff, MAX);
    //sleep(30);
    /*if (recv_packet(buff, clients[0].socket, &clients_set, clients[0].f_r))
    {
        return false;
    }
    if (buff[0] == MAXWORD)
    {
        debug_print("MAXWORD PACKET");
        printf("Player bet on %d words\n", buff[1]);
        game.rounds[game.roundIndex].maxWord = buff[1];
    }
    else
    {
        debug_print("WRONG PACKET");
        game.rounds[game.roundIndex].maxWord = 5;
    }*/
    return false;
}

bool play_round()
{
    if (ask_maxword())
    {
        return true;
    }
    return false;
}

bool play_game()
{
    uint8_t i = 0;
    for (i = 0; i < 5; i++)
    {
        if (play_round())
        {
            return false;
        }
        if (send_score())
        {
            return true;
        }
        if (swap_player())
        {
            return true;
        }
        if (sending_player_turn())
        {
            return true;
        }
    }
    return false;
}

int main(int argc, char **argv)
{

    if (argc != 2)
    {
        errmsgf("arg err\n");
        return EXIT_FAILURE;
    }
    SERVERPORT = (uint32_t)atoi(argv[1]);

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

    if (initGame(&game))
    {
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

    if (sending_player_turn())
    {
        errmsgf("err waiting\n");
        return EXIT_FAILURE;
    }

    if (play_game())
    {
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