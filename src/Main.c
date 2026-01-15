#include "../include/Main.h"

int main()
{

    InitEvalMasks();
    InitAttacks();
    ParseFEN(kiwipete);
    PrintBitBoard(occupied);
    TraceEvaluation();
    CleanupMagic();

    return 0;
}
