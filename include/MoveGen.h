#ifndef MOVEGEN_H
#define MOVEGEN_H
#include "Masks.h"
#include "MyTypes.h"
#include "Attack.h"
#include "BitBoard.h"
#include "Perft.h"

extern Move moveList[256];
extern int moveCount;
extern U64 knightMoves[64];
extern U64 kingMoves[64];

// Function prototypes
void GeneratePawnMoves(U64 pawns, U64 ownPieces, U64 enemyPieces, int side,int piece);
void GenerateKnightMoves(U64 knights, U64 ownPieces, U64 enemyPieces, int piece);
void GenerateKingMoves(U64 king, U64 ownPieces, U64 enemyPieces, int piece);
void GenerateRookMoves(U64 rooks, U64 ownPieces, U64 enemyPieces, int piece);
void GenerateBishopMoves(U64 bishops, U64 ownPieces, U64 enemyPieces, int piece);
void GenerateQueenMoves(U64 queen, U64 ownPieces, U64 enemyPieces, int piece);
void GenerateMovesInternal(U64 P, U64 N, U64 B, U64 R, U64 Q, U64 K, U64 ownPieces, U64 enemyPieces, int side);
void GenerateMoves();
void ResetMoveList();
void InitKnightMoves();
void InitKingMoves();
void InitTables();
int  DetectCapture(int to);
void AddPromotionMoves(int from, int to, int captured, int side);
void PrintMoveList();

#endif