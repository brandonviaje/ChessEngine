#include "../include/Main.h"

int main()
{
    InitAttacks();
    ParseFEN("rnbq1bnr/pppkpppp/3pP3/8/8/8/PPPP1PPP/RNBQKBNR b KQ - 0 3");
    PrintBitBoard(occupied);
    PerftDivide(1);
    CleanupMagic();
    return 0;
}