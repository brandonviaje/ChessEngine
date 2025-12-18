#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "Masks.h"
#include "MyTypes.h"
#include "Attack.h"
#include "Magic.h"
#include "Board.h"

typedef struct MoveList
{
    Move moves[MAX_MOVES];
    int count;
} MoveList;

// Function prototypes
void GeneratePawnMoves(U64 pawns, U64 ownPieces, U64 enemyPieces, int side, int piece, MoveList *list);
void GenerateKnightMoves(U64 knights, U64 ownPieces, U64 enemyPieces, int piece, MoveList *list);
void GenerateKingMoves(U64 king, U64 ownPieces, U64 enemyPieces, int piece, MoveList *list);
void GenerateRookMoves(U64 rooks, U64 ownPieces, U64 enemyPieces, int piece, MoveList *list);
void GenerateBishopMoves(U64 bishops, U64 ownPieces, U64 enemyPieces, int piece, MoveList *list);
void GenerateQueenMoves(U64 queen, U64 ownPieces, U64 enemyPieces, int piece, MoveList *list);
void GenerateMovesInternal(U64 Pawn, U64 Knight, U64 Bishop, U64 Rook, U64 Queen, U64 King, U64 ownPieces, U64 enemyPieces, int side, MoveList *list);
void GenerateMoves(MoveList *list);
void ResetMoveList(MoveList *list);
int DetectCapture(int to);
void AddPromotionMoves(int from, int to, int captured, int side, MoveList *list);
void AddMove(MoveList *list, Move m);
void PrintMoveList(MoveList *list);
int IsInCheck();

#endif
