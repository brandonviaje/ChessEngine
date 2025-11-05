#ifndef MOVEGEN_H
#define MOVEGEN_H
#include "Masks.h"
#include "BitBoard.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
    int from;    // square index the piece moves from
    int to;      // square index the piece moves to
    int flags;   // bitmask for promotions, en passant, castling, etc.
} Move;

extern Move moveList[256];
extern int moveCount;

// Function prototypes
void GeneratePawnMoves(U64 pawns, U64 ownPieces, U64 enemyPieces, int side);
void GenerateKnightMoves(U64 knights, U64 ownPieces, U64 enemyPieces);
void GenerateKingMoves(U64 king, U64 ownPieces, U64 enemyPieces);
void GenerateRookMoves(U64 rooks, U64 ownPieces, U64 enemyPieces);
void GenerateBishopMoves(U64 bishops, U64 ownPieces, U64 enemyPieces);
void GenerateQueenMoves(U64 queen, U64 ownPieces, U64 enemyPieces);
void ResetMoveList();
void GenerateAllMoves(U64 P, U64 N, U64 B, U64 R, U64 Q, U64 K, U64 ownPieces, U64 enemyPieces, int side);

#endif