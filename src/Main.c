#include "../include/Main.h"

int main()
{
    InitAttacks();
    ParseFEN(starting_position);
    PrintBitBoard(occupied);
    for (int i = 0; i <= 6; i++)
    {
        U64 nodes = Perft(i);
        printf("Depth %d Nodes: %llu\n", i, nodes);
    }
    CleanupMagic();
    return 0;
}
