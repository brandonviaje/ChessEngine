#include "../include/Main.h"

int main()
{
    InitAttacks();
    ParseFEN(kiwipete);
    PrintBitBoard(occupied);
    for (int i = 0; i <= 4; i++)
    {
        U64 nodes = Perft(i);
        printf("Depth %d Total Nodes: %llu \n", i, nodes);
    }
    CleanupMagic();
    return 0;
}
