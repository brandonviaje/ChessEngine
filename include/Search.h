#ifndef SEARCH_H
#define SEARCH_H

#include "Board.h"
#include "MoveGen.h"
#include "Evaluate.h"
#include "MyTypes.h"
#include "Board.h"

#define INF 50000
#define MATE 49000

typedef struct
{
    long nodes;
    int quit;
} S_SEARCHINFO;

extern S_SEARCHINFO info;

void ClearSearch();
int AlphaBeta(int alpha, int beta, int depth);
void SearchPosition(int depth);

#endif