#include "errhandlers.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "net.h"
#include <sys/select.h>
#include "enumvalue.h"

#define MAX 1024

struct Game game;
struct sockaddr_in serv_addr;
struct Client_info clients[2];
int nb_client = 0;
int listen_s;
uint16_t SERVERPORT = 7777;
char SERVERIP[16] = "127.0.0.1";
fd_set listen_set;

bool swap_player()
{
    struct Client_info tmp = clients[0];
    clients[0] = clients[1];
    clients[1] = tmp;
    return false;
}

void close_client(
    struct Client_info client)
{
    if (close_socket(&(client.socket)))
    {
        debug_print("err close socket\n");
    }
    if (fclose(client.f_w) == EOF)
    {
        debug_print("err close file descriptor\n");
    }
}

void close_listen()
{
    if (close_socket(&listen_s))
    {
        debug_print("err close socket\n");
    }
}

void close_all_socket()
{
    if (close_socket(&listen_s))
    {
        debug_print("err close socket\n");
    }

    if (close_socket(&(clients[0].socket)))
    {
        debug_print("err close socket\n");
    }

    if (close_socket(&(clients[1].socket)))
    {
        debug_print("err close socket\n");
    }
    if (fclose(clients[0].f_w) == EOF)
    {
        debug_print("err close file descriptor\n");
    }
    if (fclose(clients[0].f_r) == EOF)
    {
        debug_print("err close file descriptor\n");
    }
    if (fclose(clients[1].f_w) == EOF)
    {
        debug_print("err close file descriptor\n");
    }
    if (fclose(clients[1].f_r) == EOF)
    {
        debug_print("err close file descriptor\n");
    }
}

bool check_client()
{
    size_t readV = 0;
    uint8_t tmp;
    int r;
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    FD_ZERO(&listen_set);
    FD_SET(clients[0].socket, &listen_set);

    r = select(clients[0].socket + 1, &listen_set, NULL, NULL, &timeout);

    if (r == -1)
        return true;
    if (r > 0)
    {
        if (FD_ISSET(clients[0].socket, &listen_set))
        {
            readV = fread(&tmp, sizeof(uint8_t), 1, clients[0].f_r);
            if (readV == 0)
            {
                message_print("One player disconnected\n");
                return true;
            }
        }
    }
    return false;
}

bool handle_connection()
{
    int r;
    message_print("Waiting player to connect\n");
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    while (true)
    {
        if (nb_client < 2)
        {
            if (nb_client == 1)
            {
                if (check_client())
                {
                    debug_print("Client disconnected\n");
                    close_client(clients[0]);
                    nb_client--;
                    message_print("Number of player %d/2\n", nb_client);
                }
            }
            FD_ZERO(&listen_set);
            FD_SET(listen_s, &listen_set);
            r = select(listen_s + 1, &listen_set, NULL, NULL, &timeout);
            if (r == -1)
                return true;
            if (r > 0)
            {
                if (FD_ISSET(listen_s, &listen_set))
                {
                    if (accept_client(&(clients[nb_client].socket), &listen_s, (struct sockaddr *)&(clients[nb_client].addr), &(clients[nb_client].s_len)))
                    {
                        debug_print("err accept\n");
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
                        message_print("One player connected\n");
                        nb_client++;
                        message_print("Number of player %d/2\n", nb_client);
                    }
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
    struct Packet p;
    uint8_t m[32];
    struct Message msg;
    int y;
    int size;

    for (y = 0; y < 2; y++)
    {
        size = sprintf((char *)m, "Your are player %d", y + 1);
        if (size < 0)
        {

            return true;
        }

        if (initMsg(&msg, m, (uint8_t)size))
        {

            return true;
        }

        if (set_packet(&p, (uint8_t *)&msg, sizeof(struct Message), MSG))
        {

            return true;
        }

        if (send_to(&p, clients, y, &listen_s))
        {

            return true;
        }
    }
    return false;
}

bool send_score()
{
    struct Packet p;
    uint8_t m[32];
    struct Message msg;
    int y;
    int size;
    for (y = 0; y < 2; y++)
    {
        size = sprintf((char *)m, "Your score %d", game.score);
        if (size < 0)
        {
            errmsgf("sprintf\n");
            return true;
        }

        if (initMsg(&msg, m, (uint8_t)size))
        {
            errmsgf("init\n");
            return true;
        }

        if (set_packet(&p, (uint8_t *)&msg, sizeof(struct Message), MSG))
        {
            return true;
        }

        if (send_to(&p, clients, y, &listen_s))
        {
            return true;
        }
    }
    return false;
}

bool send_end()
{
    struct Packet p;
    uint8_t m[32];
    struct Message msg;
    int y;
    int size;
    for (y = 0; y < 2; y++)
    {
        size = sprintf((char *)m, "Game over \n Your final score %d", game.score);
        if (size < 0)
        {
            errmsgf("sprintf\n");
            return true;
        }

        if (initMsg(&msg, m, (uint8_t)size))
        {
            errmsgf("init\n");
            return true;
        }

        if (set_packet(&p, (uint8_t *)&msg, sizeof(struct Message), MSG))
        {
            return true;
        }

        if (send_to(&p, clients, y, &listen_s))
        {
            return true;
        }
    }
    return false;
}

bool send_bet()
{
    struct Packet p;
    uint8_t m[32];
    struct Message msg;
    int y;
    int size;
    for (y = 0; y < 2; y++)
    {
        size = sprintf((char *)m, "Player 1 bet on %d words\n", game.rounds[game.roundIndex].maxWord);
        if (size < 0)
        {
            errmsgf("sprintf\n");
            return true;
        }

        if (initMsg(&msg, m, (uint8_t)size))
        {
            errmsgf("init\n");
            return true;
        }

        if (set_packet(&p, (uint8_t *)&msg, sizeof(struct Message), MSG))
        {
            return true;
        }

        if (send_to(&p, clients, y, &listen_s))
        {
            return true;
        }
    }
    return false;
}

bool ask_maxword()
{
    struct Packet p;
    if (set_packet(&p, p.data + 1, 1, MAXWORD))
    {
        return true;
    }
    if (send_to(&p, clients, 0, &listen_s))
    {
        return true;
    }
    bzero(p.data, MAX);

    //sleep(30);
    if (recv_from(&p, clients, 0, &listen_s))
    {
        return true;
    }
    if (p.data[0] == MAXWORD)
    {
        message_print("Player bet on %d words\n", p.data[1]);
        game.rounds[game.roundIndex].maxWord = p.data[1];
    }
    else
    {
        debug_print("WRONG PACKET\n");
        return true;
    }
    return false;
}
bool send_retry()
{
    struct Packet p;
    uint8_t m[32];
    struct Message msg;

    int size;

    size = sprintf((char *)m, "The hint can't be the guess\n");
    if (size < 0)
    {
        errmsgf("sprintf\n");
        return true;
    }

    if (initMsg(&msg, m, (uint8_t)size))
    {
        errmsgf("init\n");
        return true;
    }

    if (set_packet(&p, (uint8_t *)&msg, sizeof(struct Message), MSG))
    {
        return true;
    }

    if (send_to(&p, clients, 0, &listen_s))
    {
        return true;
    }

    return false;
}

bool ask_word()
{
    struct Packet p;
    if (set_packet(&p, p.data + 1, sizeof(struct Word), WORD))
    {
        return true;
    }
    if (send_to(&p, clients, 0, &listen_s))
    {
        return true;
    }
    bzero(p.data, MAX);

    if (recv_from(&p, clients, 0, &listen_s))
    {
        return true;
    }
    if (p.data[0] == WORD)
    {
        if (memcpy(&(game.rounds[game.roundIndex].word), p.data + 1, sizeof(struct Word)) == NULL)
            return true;
        message_print("Player 1 choose word : %s\n", game.rounds[game.roundIndex].word.word);
    }
    else
    {
        debug_print("WRONG PACKET\n");
        return true;
    }
    return false;
}

bool ask_hint()
{
    struct Packet p;
    if (set_packet(&p, p.data + 1, sizeof(struct Word), WHINT))
    {
        return true;
    }
    if (send_to(&p, clients, 0, &listen_s))
    {
        return true;
    }
    bzero(p.data, MAX);

    if (recv_from(&p, clients, 0, &listen_s))
    {
        return true;
    }
    if (p.data[0] == WHINT)
    {
        struct Word tmp;
        if (memcpy(&tmp, p.data + 1, sizeof(struct Word)) == NULL)
        {
            return true;
        }
        if (strcmp((char *)tmp.word, (char *)game.rounds[game.roundIndex].word.word) == 0)
        {
            if (send_retry())
            {
                return true;
            }
            if (ask_hint())
            {
                return true;
            }
            return false;
        }
        if (addWord(&(game.rounds[game.roundIndex].wordsHint), tmp.word, tmp.size))
        {
            debug_print("addword\n");
            return true;
        }
        message_print("Player 1 give the hint : %s\n", tmp.word);
        game.rounds[game.roundIndex].wordHintIndex++;
    }
    else
    {
        debug_print("WRONG PACKET\n");
        return true;
    }
    return false;
}

bool ask_guess()
{
    struct Packet p;

    if (set_packet(&p, p.data + 1, sizeof(struct Word), WGUESS))
    {
        return true;
    }

    if (send_to(&p, clients, 1, &listen_s))
    {
        return true;
    }
    bzero(p.data, MAX);

    if (recv_from(&p, clients, 1, &listen_s))
    {
        return true;
    }
    if (p.data[0] == WGUESS)
    {

        struct Word tmp;
        if (memcpy(&tmp, p.data + 1, sizeof(struct Word)) == NULL)
        {
            return true;
        }
        if (addWord(&(game.rounds[game.roundIndex].wordsGuess), tmp.word, tmp.size))
        {
            return true;
        }
        message_print("Player 2 guess : %s\n", (char *)tmp.word);
        game.rounds[game.roundIndex].wordHintIndex++;
    }
    else
    {
        debug_print("WRONG PACKET\n");
        return true;
    }
    return false;
}

bool send_hint()
{
    struct Packet p;
    if (set_packet(&p, (uint8_t *)&(game.rounds[game.roundIndex].wordsHint), sizeof(struct WordList), HLIST))
    {
        return true;
    }
    if (send_to(&p, clients, 0, &listen_s))
    {
        return true;
    }
    if (send_to(&p, clients, 1, &listen_s))
    {
        return true;
    }
    return false;
}

bool send_guess()
{
    struct Packet p;
    if (set_packet(&p, (uint8_t *)&(game.rounds[game.roundIndex].wordsGuess), sizeof(struct WordList), GLIST))
    {
        return true;
    }
    if (send_to(&p, clients, 0, &listen_s))
    {
        return true;
    }
    if (send_to(&p, clients, 1, &listen_s))
    {
        return true;
    }
    return false;
}

bool round_win()
{
    struct Packet p;
    game.score += (uint8_t)(11 - game.rounds[game.roundIndex].maxWord);
    if (set_packet(&p, NULL, 0, WIN))
    {
        return true;
    }
    if (send_to(&p, clients, 0, &listen_s))
    {
        return true;
    }
    if (send_to(&p, clients, 1, &listen_s))
    {
        return true;
    }
    return false;
}

bool round_lose()
{
    struct Packet p;
    if (set_packet(&p, NULL, 0, LOSE))
    {
        return true;
    }
    if (send_to(&p, clients, 0, &listen_s))
    {
        return true;
    }
    if (send_to(&p, clients, 1, &listen_s))
    {
        return true;
    }
    return false;
}

bool guess_phase()
{

    size_t i;

    for (i = 0; i < game.rounds[game.roundIndex].maxWord; i++)
    {

        if (ask_hint())
        {
        }

        if (send_hint())
        {
        }

        if (ask_guess())
        {
        }

        if (send_guess())
        {
        }
        if (strcmp((char *)game.rounds[game.roundIndex].wordsGuess.words[game.rounds[game.roundIndex].wordGuessIndex].word, (char *)game.rounds[game.roundIndex].word.word) == 0)
        {
            if (round_win())
            {
                return true;
            }
            return false;
        }
    }
    if (round_lose())
    {
        return true;
    }
    return false;
}

bool play_round()
{
    if (ask_maxword())
    {
        return true;
    }
    if (send_bet())
    {
        return true;
    }
    if (ask_word())
    {
        return true;
    }
    if (guess_phase())
    {
        return true;
    }
    game.roundIndex++;
    return false;
}

bool play_game()
{
    uint8_t i = 0;
    for (i = 0; i < NROUND; i++)
    {
        if (sending_player_turn())
        {

            return true;
        }
        if (play_round())
        {

            return true;
        }

        if (send_score())
        {

            return true;
        }
        if (swap_player())
        {

            return true;
        }
    }
    if (send_end())
    {
        return true;
    }
    return false;
}

int main(int argc, char **argv)
{

    if (argc > 4)
    {
        errmsgf("Too many Argument\n");
        return EXIT_FAILURE;
    }
    if (argc < 3)
    {
        errmsgf("Too few Argument\n");
        return EXIT_FAILURE;
    }
    SERVERPORT = (uint16_t)atoi(argv[2]);
    NROUND = atoi(argv[1]);

    if (argc == 4)
    {
        if (strcpy(SERVERIP, argv[3]) == NULL)
        {
            errmsgf("Invalid IP adress for server\n");
            return true;
        }
    }

    message_print("Server listening %s:%d\n", SERVERIP, SERVERPORT);

    if (!((NROUND >= 1) && (NROUND <= 5)))
    {
        errmsgf("The number of rounds should be between 1 and 5\n");
        return EXIT_FAILURE;
    }

    if (make_sockaddr(&serv_addr, SERVERIP, SERVERPORT))
    {
        errmsgf("err make sockaddr\n");
        return EXIT_FAILURE;
    }

    if (init_socket(&listen_s))
    {
        errmsgf("err init socket\n");
        return EXIT_FAILURE;
    }
    debug_print("socket %d %p\n", listen_s, &listen_s);
    if (initGame(&game))
    {
        return EXIT_FAILURE;
    }

    FD_ZERO(&listen_set);

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
        close_all_socket();
        errmsgf("handle connection\n");
        return EXIT_FAILURE;
    }

    if (play_game())
    {
        close_all_socket();
        return EXIT_FAILURE;
    }

    close_all_socket();

    return EXIT_SUCCESS;
}