#include "../include/Main.h"

int main()
{
    InitAttacks();
    InitEvalMasks();

    printf("--- TIMED SEARCH TEST ---\n\n");
    
    // kiwipete position again
    ParseFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    PrintBitBoard(occupied);

    // search max depth 99, but limit time to 1sec
    printf("Searching for exactly 1 second...\n");
    SearchPosition(150, 1000); 

    return 0;
}
