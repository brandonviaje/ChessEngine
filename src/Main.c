#include "../include/Main.h"

int main()
{
    InitAttacks();
    ParseFEN(pawn_promotion);
    MoveList list;
    GenerateMoves(&list);
    PrintMoveList(&list);
    PrintBitBoard(occupied);
    CleanupMagic();
    return 0;
}
