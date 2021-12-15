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
struct Server_info serv_info;

struct sockaddr_in my_addr, serv_addr;
uint32_t SERVERPORT = 7777;
fd_set server_set;

int max(int a, int b)
{
    return a > b ? a : b;
}

int other_player(int a)
{
    return a == 0 ? 1 : 0;
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
            serv_info.f_r = fdopen(serv_info.socket, "r+");
            if (serv_info.f_r == NULL)
            {
                return true;
            }
            serv_info.f_w = fdopen(serv_info.socket, "w+");
            if (serv_info.f_w == NULL)
            {
                return true;
            }
            FD_SET(serv_info.socket, &server_set);
            debug_print("Connected\n");
            return false;
        }
    }
}

bool print_server_msg(
    uint8_t *buff)
{
    struct Word msg;
    if (buff == NULL)
    {
        errmsgf("Server msg empty\n");
        return true;
    }
    memcpy(&msg, buff, sizeof(struct Word));
    printf("[SERVER] : %s \n", msg.word);
    return false;
}

bool print_word_list(
    uint8_t *buff, uint8_t type)
{
    struct WordList msg;
    if (buff == NULL)
    {
        errmsgf("Server list empty\n");
        return true;
    }

    memcpy(&msg, buff, sizeof(struct WordList));
    if (type == GLIST)
    {
        printf("[SERVER] : Word Guess Recap \n");
    }
    else
    {
        printf("[SERVER] : Word Hint Recap \n");
    }
    uint8_t i = 0;
    for (i = 0; i < 10;)
    {
        printf("[SERVER] : Word %d | %s \n", (i + 1), msg.words[i].word);
        sleep(1);
    }
    return false;
}

bool ask_maxword()
{
    uint8_t buff[MAX];
    uint8_t maxword;
    printf("Take your bet (max 10) :\n");
    char line[256];
    if (fgets(line, sizeof(line), stdin))
    {
        fflush(stdin);
        debug_print("%s", line);
        int tmp = 5;
        if (sscanf(line, "%d", &tmp) == 1)
        {
            if (tmp > 10)
                tmp = 10;
            printf("You bet on %d words\n", maxword);
            return false;
        }
        maxword = tmp;
    }
    if (set_packet(buff, &maxword, sizeof(uint8_t), MAXWORD))
    {
        return true;
    }
    if (send_packet(buff, serv_info.socket, &server_set, serv_info.f_w))
    {
        return true;
    }
    return false;
}

bool process_packet(uint8_t *buff, uint8_t type)
{

    switch (type)
    {
    case WHINT:
        break;
    case WGUESS:
        break;
    case MAXWORD:
        if (ask_maxword())
        {
            return true;
        }
        break;
    case GLIST:
        if (print_word_list(buff, type))
        {
            return true;
        }
        break;
    case HLIST:
        if (print_word_list(buff, type))
        {
            return true;
        }
        break;
    case MSG:
        if (print_server_msg(buff))
        {
            return true;
        }
        break;
    case FAIL:
        printf("You have exhausted all you attempt.\n");
        break;
    case SCORE:
        printf("Your score is %d \n", buff[0]);
        break;
    case PING:
        break;
    default:
        debug_print("Unknown packet\n");
        break;
    }
    return false;
}

bool waiting_data()
{
    uint8_t buff[MAX];
    while (true)
    {
        if (recv_packet(buff, serv_info.socket, &server_set, serv_info.f_r))
        {
            return true;
        }
        if (process_packet(buff + 1, buff[0]))
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

    if (handle_connection())
    {
        return EXIT_FAILURE;
    }

    if (waiting_data())
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