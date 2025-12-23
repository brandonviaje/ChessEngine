#include "../include/Main.h"

int main()
{
    InitAttacks();
    // ParseFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/P1N2Q2/1PPBBPpP/R3K2R w KQkq - 0 2");
    ParseFEN("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
    PrintBitBoard(occupied);
    for(int i = 0; i<=7; i++){
        U64 nodes = Perft(i);
        printf("Depth %d Nodes: %llu\n",i,nodes);
    }
    CleanupMagic();
    return 0;
}
