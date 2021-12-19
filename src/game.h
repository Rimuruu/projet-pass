#pragma once
#include <stdint.h>
#include <string.h>
#include "errhandlers.h"
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>
#include "enumvalue.h"

#define MAXMSG 256
#define MAXW 32

extern int NROUND;

struct Word
{
    uint8_t word[MAXW];
    uint8_t size;
} __attribute__((__packed__));

struct Message
{
    uint8_t msg[MAXMSG];
    uint8_t size;
} __attribute__((__packed__));

struct WordList
{
    struct Word words[10];
    uint8_t size;
} __attribute__((__packed__));

struct Round
{
    struct WordList wordsHint;
    struct WordList wordsGuess;
    struct Word word;
    uint8_t wordHintIndex;  // Last word Hint;
    uint8_t wordGuessIndex; // Last word guess;
    uint8_t maxWord;        // Max guess allow
};

struct Game
{
    struct Round rounds[5];
    uint8_t roundIndex; // Current round
    uint8_t score;
};

bool initGame(struct Game *game);
bool initRound(struct Round *round);
bool initWord(struct Word *word,
              uint8_t *word_p, uint8_t size);
bool initMsg(struct Message *msg,
             uint8_t *msg_p, uint8_t size);
bool initWordList(struct WordList *wordlist);
bool addWord(struct WordList *wordlist,
             uint8_t *word, uint8_t size);

bool print_Word(struct Word *word);
bool print_Round(struct Round *round);
bool print_Game(struct Game *game);