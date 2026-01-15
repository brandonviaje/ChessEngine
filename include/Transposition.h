#ifndef TT_H
#define TT_H

#include "Main.h"

// TT Entry Flags
#define TT_FLAG_EXACT 0 // exact score
#define TT_FLAG_ALPHA 1 // Upper Bound
#define TT_FLAG_BETA 2  // Lower Bound

typedef struct
{
    U64 hashKey; //  zobrist Key
    int score;   //  evaluation score
    int move;    //  best move found
    int depth;
    int flags; // EXACT, ALPHA, or BETA
} TTEntry;

typedef struct
{
    TTEntry *entries;
    int numEntries;
} TranspositionTable;

extern TranspositionTable TTable;

// Functions
void InitTT(int mbSize);
void ClearTT();
int ReadTT(U64 positionKey, int *move, int *score, int alpha, int beta, int depth);
void WriteTT(U64 positionKey, int move, int score, int depth, int flag);

#endif