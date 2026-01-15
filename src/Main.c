#include "../include/Main.h"

int main()
{
    InitAttacks();
    InitEvalMasks();
    InitZobrist();

    ParseFEN(kiwipete);
    
    printf("Engine initialized. Running PERFT...\n");

    for(int i = 1; i <= 6; i++)
    {
        clock_t start = clock();
        U64 nodes = Perft(i);
        clock_t end = clock();

        double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
        if (time_taken <= 0) time_taken = 0.001; 

        printf("Depth %d: %12llu nodes | %8.2f sec | %10.0f NPS\n", i, nodes, time_taken, nodes / time_taken);
    }
    return 0;
}
