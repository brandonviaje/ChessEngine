#include "../include/Main.h"

void InitAll()
{
    InitAttacks();
    InitEvalMasks();
    InitZobrist();
    InitTT(64);
}

int main()
{
    InitAll();
    UciLoop();
    CleanupMagic();

    return 0;
}
