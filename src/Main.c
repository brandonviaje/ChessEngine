#include "../include/Main.h"

int main()
{
    InitAttacks();
    ParseFEN(starting_position);
    
    for (int depth = 1; depth <= 4; depth++) {
        U64 nodes = Perft(depth);
        printf("Perft depth %d: %llu nodes\n", depth, nodes);
    }
    
    CleanupMagic();
    return 0;
}
