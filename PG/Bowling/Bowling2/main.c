#include "board.h"

int main(void)
{
    Score player1;
    initScore(&player1, "player1");

    for (int i = 1; i <= 9;++i){
        playGame(&player1, i);
    }
    playGame10Frame(&player1);
}