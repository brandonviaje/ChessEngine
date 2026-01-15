#ifndef SEARCH_H
#define SEARCH_H

#include "Board.h"
#include "Evaluate.h"
#include "MoveGen.h"
#include "MovePicker.h"
#include "MyTypes.h"
#include <sys/time.h>
#include "Transposition.h"

#define INF 50000
#define MATE 49000

typedef struct
{
    long nodes;
    int startTime;
    int stopTime;
    int depth;
    int stopped;
} S_SEARCHINFO;

extern S_SEARCHINFO info;
int GetTimeMs();
void CheckTime();
void ClearSearch();
int AlphaBeta(int alpha, int beta, int depth);
void SearchPosition(int depth, int timeTime);
int Quiescence(int alpha, int beta);

#endif
