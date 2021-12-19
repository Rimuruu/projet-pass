#include "game.h"

int NROUND = 0;

bool initGame(struct Game *game)
{
    uint32_t i;
    if (game == NULL)
    {
        errmsgf("Game is null");
        return true;
    }
    game->roundIndex = 0;
    game->score = 0;

    for (i = 0; i < NROUND; i++)
    {
        if (initRound(game->rounds + i))
        {
            errmsgf("Init Round error");
            return true;
        }
    }
    return false;
}

bool initRound(struct Round *round)
{
    uint32_t i;
    if (round == NULL)
    {
        errmsgf("Round null");
        return true;
    }
    round->wordHintIndex = 0;
    round->wordGuessIndex = 0;
    round->maxWord = 0;
    for (i = 0; i < 10; i++)
    {
        if (initWordList(&(round->wordsGuess)))
        {
            errmsgf("Init Word error");
            return true;
        }
        if (initWordList(&(round->wordsHint)))
        {
            errmsgf("Init Word error");
            return true;
        }
    }
    if (initWord(&(round->word), (uint8_t *)"-", 1))
    {
        return true;
    }
    return false;
}

bool initWord(struct Word *word,
              uint8_t *word_p, uint8_t size)
{
    if (word == NULL)
    {
        errmsgf("Word null");
        return true;
    }
    if (size > MAXW)
    {
        errmsgf("Word too long");
        return true;
    }
    bzero(word->word, 32);
    if (memcpy(word->word, word_p, size) == NULL)
    {
        errmsgf("strcpy error");
        return true;
    }
    word->size = size;
    return false;
}

bool initMsg(struct Message *msg,
             uint8_t *msg_p, uint8_t size)
{
    if (msg == NULL)
    {
        errmsgf("msg null");
        return true;
    }
    if (size > MAXMSG)
    {
        errmsgf("Msg too long");
        return true;
    }
    bzero(msg->msg, MAXMSG);
    if (memcpy(msg->msg, msg_p, size) == NULL)
    {
        errmsgf("strcpy error");
        return true;
    }
    msg->size = size;
    return false;
}

bool initWordList(struct WordList *wordlist)
{
    if (wordlist == NULL)
    {
        errmsgf("Word null");
        return true;
    }
    uint8_t i;
    for (i = 0; i < 10; i++)
    {
        if (initWord(&(wordlist->words[i]), (uint8_t *)"-", 1))
        {
            return true;
        }
    }

    wordlist->size = 0;

    return false;
}
bool addWord(struct WordList *wordlist, uint8_t *word, uint8_t size)
{
    if (wordlist->size == 10)
    {
        return true;
    }
    if (initWord(&(wordlist->words[wordlist->size]), word, size))
    {
        return true;
    }
    wordlist->size += 1;
    return true;
}

bool print_Word(struct Word *word)
{
    if (word == NULL)
    {
        errmsgf("Word null");
        return true;
    }
    debug_print("Word %s size %" PRIu32 " \n", (char *)word->word, word->size);
    return false;
}

bool print_Round(struct Round *round)
{
    uint32_t i;
    if (round == NULL)
    {
        errmsgf("Round null");
        return true;
    }
    debug_print("wordHintIndex %" PRIu32 " wordGuessIndex %" PRIu32 " maxWord %" PRIu32 " \n", round->wordHintIndex, round->wordGuessIndex, round->maxWord);
    for (i = 0; i < 10; i++)
    {

        debug_print("Word tips %" PRIu32 " ", i);
        if (print_Word(round->wordsHint.words + i))
            return false;
        debug_print("Word guess %" PRIu32 " ", i);
        if (print_Word(round->wordsGuess.words + i))
            return false;
    }
    return false;
}
bool print_Game(struct Game *game)
{
    uint32_t i;
    if (game == NULL)
    {
        errmsgf("Game is null");
        return true;
    }
    debug_print("Score %" PRIu32 " Round %" PRIu32 " \n", game->score, game->roundIndex);

    for (i = 0; i < NROUND; i++)
    {
        debug_print("Word round %" PRIu32 " ", i);
        if (print_Round(game->rounds + i))
            return false;
    }
    return false;
}