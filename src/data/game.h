#include <stdint.h>
#include <string.h> 
#include "errhandlers.h"
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>

#pragma once

struct Word{
    char word[32];
    uint32_t size;
}__attribute__((__packed__));



struct Round{
    struct Word wordsHint[10];
    struct Word wordsGuess[10];
    uint32_t wordHintIndex; // Last word Hint;
    uint32_t wordGuessIndex; // Last word guess;
    uint32_t maxWord; // Max guess allow

}__attribute__((__packed__)); // Structures que l'on veut envoyé dans le réseau 



struct Game {
    struct Round rounds[5];
    uint32_t roundIndex; // Current round
    uint32_t score;

}__attribute__((__packed__));


bool initGame (struct Game* game);
bool initRound (struct Round* round);
bool initWord (struct Word* word,
                char* word_p);




bool print_Word(struct Word* word);
bool print_Round(struct Round* round);
bool print_Game(struct Game* game);