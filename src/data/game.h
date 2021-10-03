#include <stdint.h>
#include <string.h> 
#include "errhandlers.h"
#include <stdio.h>
#include <stdbool.h>
struct Word{
    char word[512];
    uint32_t size;
};



struct Round{
    struct Word wordsHint[16];
    struct Word wordsGuess[16];
    uint32_t wordHintIndex; // Last word Hint;
    uint32_t wordGuessIndex; // Last word guess;
    uint32_t maxWord; // Max guess allow

};



struct Game {
    struct Round rounds[5];
    uint32_t roundIndex; // Current round
    uint32_t score;

};


bool initGame (struct Game* game);
bool initRound (struct Round* round);
bool initWord (struct Word* word,
                char* word_p);




bool print_Word(struct Word* word);
bool print_Round(struct Round* round);
bool print_Game(struct Game* game);