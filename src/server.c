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
            
            errmsgf("sprintf\n");
            return true;
        }

        if (initWord(&msg, m, 32))
        {
            
            errmsgf("init\n");
            return true;
        }

        if (set_packet(buff, (uint8_t *)&msg, sizeof(struct Word), MSG))
        {
            
            errmsgf("packet\n");
            return true;
        }

        
        if(send_to(buff, clients, y)){
            
            errmsgf("packet2\n");
            return true;
        }

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
            errmsgf("sprintf\n");
            return true;
        }

        if (initWord(&msg, m, 32))
        {
            errmsgf("init\n");
            return true;
        }

        if (set_packet(buff, (uint8_t *)&msg, sizeof(struct Word), MSG))
        {
            return true;
        }

        if (send_to(buff, clients, y))
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
    if (send_to(buff, clients, 0))
    {
        return true;
    }
    bzero(buff, MAX);
    //sleep(30);
    if (recv_from(buff, clients, 0))
    {
        return true;
    }
    if (buff[0] == MAXWORD)
    {
        printf("Player bet on %d words\n", buff[1]);
        game.rounds[game.roundIndex].maxWord = buff[1];
    }
    else
    {
        debug_print("WRONG PACKET\n");
        return true;
    }
    return false;
}

bool ask_word(){
    uint8_t buff[MAX];
    if (set_packet(buff, NULL, 0, WORD))
    {
        return true;
    }
    if (send_to(buff, clients, 0))
    {
        return true;
    }
    bzero(buff, MAX);
    if (recv_from(buff, clients, 0))
    {
        return true;
    }
    if (buff[0] == WORD)
    {
        memcpy(&(game.rounds[game.roundIndex].word), buff+1, sizeof(struct Word));
        printf("Player 1 choose word : %s\n", game.rounds[game.roundIndex].word.word);
    }
    else
    {
        debug_print("WRONG PACKET\n");
        return true;
    }
    return false;


}


bool ask_hint(){
    uint8_t buff[MAX];
    if (set_packet(buff, NULL, 0, WHINT))
    {
        return true;
    }
    if (send_to(buff, clients, 0))
    {
        return true;
    }
    bzero(buff, MAX);

    if (recv_from(buff, clients, 0))
    {
        return true;
    }
    if (buff[0] == WHINT)
    {  
        struct Word tmp;
        memcpy(&tmp, buff+1, sizeof(struct Word));
        addWord(&(game.rounds[game.roundIndex].wordsHint),tmp.word,tmp.size);
        printf("Player 1 give the hint : %s\n",tmp.word);
        game.rounds[game.roundIndex].wordHintIndex++;

    }
    else
    {
        debug_print("WRONG PACKET\n");
        return true;
    }
    return false;


}

bool ask_guess(){
    uint8_t buff[MAX];
 
    if (set_packet(buff, NULL, 0, WGUESS))
    {
        return true;
    }

    if (send_to(buff, clients, 1))
    {
        return true;
    }
    bzero(buff, MAX);

    if (recv_from(buff, clients, 1))
    {
        return true;
    }
    if (buff[0] == WGUESS)
    {  

        struct Word tmp;
        memcpy(&tmp, buff+1, sizeof(struct Word));
        addWord(&(game.rounds[game.roundIndex].wordsGuess),tmp.word,tmp.size);
        printf("Player 2 guess : %s\n",(char*)tmp.word);
        game.rounds[game.roundIndex].wordHintIndex++;

    }
    else
    {
        debug_print("WRONG PACKET\n");
        return true;
    }
    return false;


}

bool send_hint(){
    uint8_t buff[MAX];
    if (set_packet(buff,(uint8_t *) &(game.rounds[game.roundIndex].wordsHint), sizeof(struct WordList), HLIST))
    {
        return true;
    }
    if (send_to(buff, clients, 0))
    {
        return true;
    }
    if (send_to(buff, clients, 1))
    {
        return true;
    }
    return false;


}

bool send_guess(){
    uint8_t buff[MAX];
    if (set_packet(buff,(uint8_t *) &(game.rounds[game.roundIndex].wordsGuess), sizeof(struct WordList), GLIST))
    {
        return true;
    }
    if (send_to(buff, clients, 0))
    {
        return true;
    }
    if (send_to(buff, clients, 1))
    {
        return true;
    }
    return false;


}
bool round_win(){
    uint8_t buff[MAX];
    game.score+=(uint8_t)(11-game.rounds[game.roundIndex].maxWord);
    if (set_packet(buff,NULL,0, WIN))
    {
        return true;
    }
    if (send_to(buff, clients, 0))
    {
        return true;
    }
    if (send_to(buff, clients, 1))
    {
        return true;
    }
    return false;

}

bool round_lose(){
    uint8_t buff[MAX];
    if (set_packet(buff,NULL,0, LOSE))
    {
        return true;
    }
    if (send_to(buff, clients, 0))
    {
        return true;
    }
    if (send_to(buff, clients, 1))
    {
        return true;
    }
    return false;

}


bool guess_phase(){
 
    size_t i;

    for(i = 0; i < game.rounds[game.roundIndex].maxWord;i++){
   
        if(ask_hint()){

        }

        if(send_hint()){

        }

        if(ask_guess()){
            
        }

        if(send_guess()){

        }
        if(strcmp((char *)game.rounds[game.roundIndex].wordsGuess.words[game.rounds[game.roundIndex].wordGuessIndex].word,(char *)game.rounds[game.roundIndex].word.word)==0){
            if(round_win()){
                return true;
            }
            return false;
            
        }
        
    }
    if(round_lose()){
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
    if(ask_word())
    {
         return true;
    }
    if(guess_phase()){
        return true;
    }
    game.roundIndex++;
    return false;
}



bool play_game()
{
    uint8_t i = 0;
    for (i = 0; i < 5; i++)
    {
        if (play_round())
        {
            errmsgf("play round\n");
            return true;
        }

        if (send_score())
        {
            errmsgf("send score\n");
            return true;
        }
        if (swap_player())
        {
            errmsgf("swap_player err\n");
            return true;
        }
  

        if (sending_player_turn())
        {
            errmsgf("sending_player_turn\n");
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
    SERVERPORT = (uint16_t)atoi(argv[1]);

    debug_print("serverport %s %d\n", IP, SERVERPORT);
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
    debug_print("socket %d\n", listen_s);
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