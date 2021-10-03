#include "game.h"

bool initGame (struct Game* game){
    uint32_t i;
    if(game == NULL){
        errmsgf("Game is null");
        return true;
    }
    game->roundIndex = 0;
    game->score = 0;
    for(i = 0; i<5; i++){
        if(initRound(game->rounds+i)){
            errmsgf("Init Round error");  
            return true;
        }

    }
    return false;
}


bool initRound (struct Round* round){
    uint32_t i;
    if(round == NULL){
        errmsgf("Round null");
        return true;
    }
    round->wordHintIndex=0;
    round->wordGuessIndex=0;
    round->maxWord=0;
    for(i = 0; i<16; i++){
        if(initWord(round->wordsGuess+i,"EMPTY")){
            errmsgf("Init Word error");  
            return true;
        }
        if(initWord(round->wordsHint+i,"EMPTY")){
            errmsgf("Init Word error");  
            return true;
        }

    }
    return false;
}

bool initWord (struct Word* word,
                char* word_p){
    if(word == NULL){
        errmsgf("Word null");
        return true;
    }
    if(strcpy(word->word,word_p) == NULL){
        errmsgf("strcpy error");
        return true;
    }
    word->size = strlen(word_p);
    return false;
}


bool print_Word(struct Word* word){
    if(word == NULL){
        errmsgf("Word null");
        return true;
    }
    printf("Word %s size %lu \n",word->word,(unsigned long)word->size);
    return false;
}

bool print_Round(struct Round* round){
    uint32_t i;
    if(round == NULL){
        errmsgf("Round null");
        return true;
    }
    printf("wordHintIndex %lu wordGuessIndex %lu maxWord %lu \n", (unsigned long)round->wordHintIndex,(unsigned long)round->wordGuessIndex, (unsigned long)round->maxWord);
    for(i = 0; i<16; i++){
        
        printf("Word tips %lu ", (unsigned long)i);
        if(print_Word(round->wordsHint+i)) return false;
        printf("Word guess %lu ",(unsigned long)i);
        if(print_Word(round->wordsGuess+i)) return false;
    }
    return false;


}
bool print_Game(struct Game* game){
    uint32_t i;
    if(game == NULL){
        errmsgf("Game is null");
        return true;
    }
    for(i = 0; i<5; i++){
        printf("Word round %lu ",(unsigned long)i);
        if(print_Round(game->rounds+i))return false;

    }
    return false;

}