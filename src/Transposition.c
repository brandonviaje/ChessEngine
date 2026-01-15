#include "../include/Transposition.h"

TranspositionTable TTable;

void InitTT(int mbSize)
{
    // Calculate number of entries that fit in the given MB size
    int bytes = mbSize * 1024 * 1024;
    TTable.numEntries = bytes / sizeof(TTEntry);

    // free old memory if exists
    if (TTable.entries != NULL)
    {
        free(TTable.entries);
    }

    // alloc new memory
    TTable.entries = (TTEntry *)malloc(TTable.numEntries * sizeof(TTEntry));

    if (TTable.entries == NULL)
    {
        printf("Error: Failed to allocate Transposition Table!\n");
        exit(1);
    }

    ClearTT();
    printf("Transposition Table initialized with %d entries (~%d MB)\n", TTable.numEntries, mbSize);
}

// clear all entries
void ClearTT()
{
    for (int i = 0; i < TTable.numEntries; i++)
    {
        TTable.entries[i].hashKey = 0;
        TTable.entries[i].score = 0;
        TTable.entries[i].move = 0;
        TTable.entries[i].depth = 0;
        TTable.entries[i].flags = 0;
    }
}

// Read from the transposition table
int ReadTT(U64 positionKey, int *move, int *score, int alpha, int beta, int depth)
{
    // get the index
    int index = positionKey % TTable.numEntries;

    // check if key matches
    if (TTable.entries[index].hashKey == positionKey)
    {
        // extract best move
        *move = TTable.entries[index].move;

        // check if depth is sufficient
        if (TTable.entries[index].depth >= depth)
        {
            *score = TTable.entries[index].score;
            int flag = TTable.entries[index].flags;

            // check flags for cutoff
            if (flag == TT_FLAG_EXACT)
            {
                return 1; // exact score, return immediately
            }
            if (flag == TT_FLAG_ALPHA && *score <= alpha)
            {
                return 1; // Upper bound is worse than alpha, useless branch
            }
            if (flag == TT_FLAG_BETA && *score >= beta)
            {
                return 1; // Lower bound is better than beta, cutoff
            }
        }
    }
    return 0; // No valid entry found
}

// Write to transposition table
void WriteTT(U64 positionKey, int move, int score, int depth, int flag)
{
    int index = positionKey % TTable.numEntries;

    TTable.entries[index].hashKey = positionKey;
    TTable.entries[index].score = score;
    TTable.entries[index].flags = flag;
    TTable.entries[index].depth = depth;
    TTable.entries[index].move = move;
}