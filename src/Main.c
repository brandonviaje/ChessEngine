#include "../include/Main.h"

int main()
{
    InitAttacks();
    InitEvalMasks();
    InitZobrist();
    InitTT(64);

    ParseFEN(kiwipete);

    CleanupMagic();

    return 0;
}
