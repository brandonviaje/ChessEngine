#ifndef EVALUATION_H
#define EVALUATION_H

#include "Evaluate.h"
#include "MyTypes.h"
#include "Board.h"
#include "Utils.h"

// Function prototypes
int MaterialAndPosition(int phase);
int PassedPawns(U64 pawns, U64 enemyPawns, int color);
int DoubledPawns(U64 pawns);
int IsolatedPawns(U64 pawns);
int BackwardPawns(U64 pawns, U64 enemyPawns, int color);
int KingPawnShield(U64 king, U64 pawns, int color);
int KingCentralization(U64 king);
int RooksOnOpenFiles(U64 rooks);
int RooksOn7th(U64 rooks, int color);
int GamePhase();
int Evaluate();
void PrintEvaluation();
void TraceEvaluation();

#endif
