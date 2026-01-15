#include "../include/Main.h"

int main()
{
    // Init Engine
    InitAttacks();
    InitEvalMasks();

    // test
    printf("--- ENGINE TEST ---\n\n");
    printf("TEST 1: Start Position (Depth 5)\n");
    ParseFEN(starting_position);
    PrintBitBoard(occupied);
    SearchPosition(5);

    printf("\n-----------------------------\n\n");
    printf("TEST 2: Scholar's Mate Tactics (Depth 4)\n");
    ParseFEN("r1bqk1nr/pppp1ppp/2n5/2b1p3/2B1P3/5Q2/PPPP1PPP/RNB1K1NR w KQkq - 4 4");
    PrintBitBoard(occupied);

    // best move is Qxf7 (Checkmate)
    SearchPosition(4);
    CleanupMagic();

    return 0;
}