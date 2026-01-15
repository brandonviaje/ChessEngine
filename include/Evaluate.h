#ifndef EVALUATION_H
#define EVALUATION_H

// Piece values
#define PAWN_VALUE 100
#define KNIGHT_VALUE 320
#define BISHOP_VALUE 330
#define ROOK_VALUE 500
#define QUEEN_VALUE 900
#define KING_VALUE 20000

#include "MyTypes.h"
#include "Board.h"
#include "Attack.h"
#include "PieceSquareTable.h"

// Function prototypes
int PopCount(U64 bb);
int Mirror(int square);
int PieceValue(int piece, int phase);
int SimpleEval(int phase);
int PassedPawns(U64 pawns, U64 enemyPawns, int color);
int DoubledPawns(U64 pawns);
int IsolatedPawns(U64 pawns);
int BackwardPawns(U64 pawns, U64 enemyPawns, int color);
int KingPawnShield(U64 king, U64 pawns, int color);
int KingCentralization(U64 king);
int PositionalEval(int phase);
int Mobility(U64 pieces, int pieceType);
int CentralizationBonus(U64 pieces, int pieceType);
int HasBishopPair(U64 bishops);
int RooksOnOpenFiles(U64 rooks);
int RooksOn7th(U64 rooks, int color);
int GamePhase();
int Evaluate();
void PrintEvaluation();
#endif
