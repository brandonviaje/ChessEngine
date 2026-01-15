#include "../include/Main.h"

int main()
{
    InitAttacks();

    ParseFEN(starting_position);
    printf("Initial Board:\n");
    PrintBitBoard(occupied);
    PrintEvaluation();

    CleanupMagic();

    return 0;
}
