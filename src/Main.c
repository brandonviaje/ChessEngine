#include "../include/Main.h"

int main()
{
    InitAttacks();
    InitEvalMasks();
    InitZobrist();
    InitTT(64);

    ParseFEN(starting_position);

    return 0;
}
