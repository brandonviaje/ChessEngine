#ifndef MOVEPICKER_H
#define MOVEPICKER_H

#include "Board.h"
#include "MyTypes.h"

int GetPieceIndex(int piece);
int SameMove(Move a, Move b);
int ScoreMove(Move m, Move k1, Move k2, int ttMove);
void PickNextMove(MoveList *list, int moveNum, Move k1, Move k2, int ttMove);

#endif
