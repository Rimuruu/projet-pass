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
uint16_t SERVERPORT = 7777;
char SERVERIP[16] = "127.0.0.1";

bool process_packet(uint8_t *buff, uint8_t type);

void close_all_socket()
{

    if (close_socket(&serv_info.socket))
    {
        debug_print("err close socket\n");
    }
    if (fclose(serv_info.f_w) == EOF)
    {
        debug_print("err close file descriptor\n");
    }
    if (fclose(serv_info.f_r) == EOF)
    {
        debug_print("err close file descriptor\n");
    }
}

bool handle_connection()
{
    while (true)
    {
        if (connection_server(&(serv_info.socket), (struct sockaddr *)&serv_addr, sizeof(serv_addr)))
        {
            message_print("Retrying\n");
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

            message_print("Connected\n");
            return false;
        }
    }
}

bool print_server_msg(
    uint8_t *buff)
{
    struct Message msg;
    if (buff == NULL)
    {
        errmsgf("Server msg empty\n");
        return true;
    }
    if (memcpy(&msg, buff, sizeof(struct Message)) == NULL)
    {
        return true;
    }
    message_print("%s \n", msg.msg);
    message_print("\n");
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

    if (memcpy(&msg, buff, sizeof(struct WordList)) == NULL)
    {
        return true;
    }
    if (type == GLIST)
    {
        message_print("Word Guess Recap \n");
    }
    else
    {
        message_print("Word Hint Recap \n");
    }
    uint8_t i = 0;
    for (i = 0; i < msg.size; i++)
    {
        message_print("Word %d | %s \n", (i + 1), msg.words[i].word);
        if (sleep(1) != 0)
        {
            debug_print("error sleep\n");
        }
    }
    message_print("\n");
    return false;
}

bool send_maxword()
{
    struct Packet p;
    uint8_t maxword;

    char line[3];
    if (fgets(line, sizeof(line), stdin) != NULL)
    {
        char *c = strchr(line, '\n');
        if (c != NULL)
        { //check exist newline
            *c = 0;
        }
        else
        {
            int rl = scanf("%*[^\n]");
            if (rl == EOF)
            {
                if (ferror(stdin))
                {
                    debug_print("err scanf %s\n", strerror(errno));
                }
            }
            int cl = scanf("%*c"); //clear upto newline
            if (cl == EOF)
            {
                if (ferror(stdin))
                {
                    debug_print("err scanf %s\n", strerror(errno));
                }
            }
        }
        debug_print("line %s\n", line);
        int tmp = 10;
        if (sscanf(line, "%d", &tmp) == 1)
        {
            if (tmp > 10 || tmp < 1)
                tmp = 10;
        }
        maxword = (uint8_t)tmp;
    }
    else
    {
        maxword = 5;
    }
    message_print("You bet on %d words\n", maxword);
    if (set_packet(&p, &maxword, sizeof(uint8_t), MAXWORD))
    {
        return true;
    }
    if (send_packet(&p, serv_info.socket, serv_info.f_w))
    {
        return true;
    }
    return false;
}

bool send_word(uint8_t type)
{
    struct Packet p;

    char line[32];
    struct Word word;
    if (initWord(&word, (uint8_t *)"Empty", 5))
    {
        return true;
    }
    if (fgets(line, sizeof(line), stdin) != NULL)
    {
        char *c = strchr(line, '\n');
        if (c != NULL)
        { //check exist newline
            *c = 0;
        }
        else
        {
            int rl = scanf("%*[^\n]");
            if (rl == EOF)
            {
                if (ferror(stdin))
                {
                    debug_print("err scanf %s\n", strerror(errno));
                }
            }
            int cl = scanf("%*c"); //clear upto newline
            if (cl == EOF)
            {
                if (ferror(stdin))
                {
                    debug_print("err scanf %s\n", strerror(errno));
                }
            }
        }
        debug_print("line %s\n", line);
        if (sscanf(line, "%s", (char *)&(word.word)) == 1)
        {
        }
    }
    message_print("You choose the word : %s\n", (char *)word.word);
    word.size = (uint8_t)strlen((char *)word.word);
    if (set_packet(&p, (uint8_t *)&word, sizeof(struct Word), type))
    {
        return true;
    }
    if (send_packet(&p, serv_info.socket, serv_info.f_w))
    {
        return true;
    }
    return false;
}

bool ask_maxword()
{
    fd_set set;
    int r;
    struct Packet p;

    int maxfd = STDIN_FILENO > serv_info.socket ? STDIN_FILENO : serv_info.socket;

    message_print("Take your bet (max 10) :\n");
    while (true)
    {
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        FD_ZERO(&set);
        FD_SET(STDIN_FILENO, &set);
        FD_SET(serv_info.socket, &set);
        r = select(maxfd + 1, &set, NULL, NULL, &timeout);
        if (r == -1)
        {
            return true;
        }
        if (r > 0)
        {
            if (FD_ISSET(STDIN_FILENO, &set))
            {
                if (send_maxword())
                {
                    return true;
                }
                return false;
            }
            if (FD_ISSET(serv_info.socket, &set))
            {
                if (recv_unknown_packet(&p, serv_info.socket, serv_info.f_r))
                {
                    if (feof(serv_info.f_r) != 0)
                    {
                        errmsgf("Game over Server disconnected\n");
                    }
                    return true;
                }
                if (process_packet(p.data + 1, p.data[0]))
                {
                    return true;
                }
            }
        }
    }
}

bool ask_word(uint8_t type)
{
    fd_set set;
    int r;
    struct Packet p;

    int maxfd = STDIN_FILENO > serv_info.socket ? STDIN_FILENO : serv_info.socket;

    switch (type)
    {
    case WGUESS:
        message_print("Take a guess (max 32 char) :\n");
        break;
    case WHINT:
        message_print("Give a hint to your partner (max 32 char) :\n");
        break;
    case WORD:
        message_print("Type the word you want to be guessing (max 32 char) :\n");
        break;
    }

    while (true)
    {
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        FD_ZERO(&set);
        FD_SET(STDIN_FILENO, &set);
        FD_SET(serv_info.socket, &set);
        r = select(maxfd + 1, &set, NULL, NULL, &timeout);
        if (r == -1)
        {
            return true;
        }
        if (r > 0)
        {
            if (FD_ISSET(STDIN_FILENO, &set))
            {
                if (send_word(type))
                {
                    return true;
                }
                return false;
            }
            if (FD_ISSET(serv_info.socket, &set))
            {
                if (recv_unknown_packet(&p, serv_info.socket, serv_info.f_r))
                {
                    if (feof(serv_info.f_r) != 0)
                    {
                        errmsgf("Game over Server disconnected\n");
                    }
                    return true;
                }
                if (process_packet(p.data + 1, p.data[0]))
                {
                    return true;
                }
            }
        }
    }
}

bool process_packet(uint8_t *buff, uint8_t type)
{

    switch (type)
    {
    case WHINT:
        if (ask_word(WHINT))
        {
            return true;
        }
        break;
    case WGUESS:
        if (ask_word(WGUESS))
        {
            return true;
        }
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
    case LOSE:
        message_print("You have exhausted all you attempt.\n");
        break;
    case WIN:
        message_print("You guessed the correct word.\n");
        break;
    case WORD:
        if (ask_word(WORD))
        {
            return true;
        }
        break;
    default:
        debug_print("Unknown packet\n");
        break;
    }
    return false;
}

bool waiting_data()
{
    struct Packet p;
    while (true)
    {
        if (recv_unknown_packet(&p, serv_info.socket, serv_info.f_r))
        {
            if (feof(serv_info.f_r) != 0)
            {
                errmsgf("Game over Server disconnected\n");
            }
            return true;
        }
        if (process_packet(p.data + 1, p.data[0]))
        {
            return true;
        }
    }
    return false;
}

int main(int argc, char **argv)
{

    if (argc > 3)
    {
        errmsgf("Too many arguments\n");
        return EXIT_FAILURE;
    }
    if (argc < 3)
    {
        errmsgf("Too few arguments\n");
        return EXIT_FAILURE;
    }
    SERVERPORT = (uint16_t)atoi(argv[1]);

    if (strcpy(SERVERIP, argv[2]) == NULL)
    {
        errmsgf("Invalid IP adress for server\n");
        return true;
    }

    message_print("Connecting to server %s:%d\n", SERVERIP, SERVERPORT);

    if (initGame(&game))
    {
        errmsgf("Init Game error\n");
        return EXIT_FAILURE;
    }

    if (make_sockaddr(&my_addr, "127.0.0.1", 0))
    {
        errmsgf("error make sockaddr\n");
        return EXIT_FAILURE;
    }

    if (make_sockaddr(&serv_addr, SERVERIP, SERVERPORT))
    {
        errmsgf("error make sockaddr\n");
        return EXIT_FAILURE;
    }

    if (init_socket(&serv_info.socket))
    {
        errmsgf("error init socket\n");
        return EXIT_FAILURE;
    }

    if (init_bind(&serv_info.socket, (struct sockaddr *)&my_addr))
    {
        errmsgf("error init bind\n");
        return EXIT_FAILURE;
    }

    if (handle_connection())
    {
        return EXIT_FAILURE;
    }

    if (waiting_data())
    {
        close_all_socket();

        return EXIT_FAILURE;
    }

    close_all_socket();

    return EXIT_SUCCESS;
}