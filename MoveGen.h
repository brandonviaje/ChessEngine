#ifndef MOVEGEN_H
#define MOVEGEN_H
#include "BitBoard.h"
#include "Masks.h"

typedef struct {
    int from;    // square index the piece moves from
    int to;      // square index the piece moves to
    int flags;   // bitmask for promotions, en passant, castling, etc.
} Move;

extern Move moveList[256];
extern int moveCount;

// Function protocol
void GeneratePawnMoves(U64 pawns, U64 ownPieces, U64 enemyPieces, int side);
void GenerateRookMoves(U64 rooks, U64 ownPieces, U64 enemyPieces, int side);
void GenerateBishopMoves(U64 bishops, U64 ownPieces, U64 enemyPieces, int side);
void GenerateKnightMoves(U64 knights, U64 ownPieces, U64 enemyPieces, int side);
void GenerateKingMoves(U64 king, U64 ownPieces, U64 enemyPieces, int side);
void GenerateQueenMoves(U64 queen, U64 ownPieces, U64 enemyPieces, int side);

#endif