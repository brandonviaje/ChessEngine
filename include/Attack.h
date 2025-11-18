#ifndef ATTACK_H
#define ATTACK_H

#include "BitBoard.h"
#include "MoveGen.h"
#include "MyTypes.h"

extern U64 knightAttacks[64];
extern U64 kingAttacks[64];
extern U64 pawnAttacks[2][64];

int IsInCheck();
int IsAttackSquare(int side, int square);
U64 GeneratePawnAttacks(int side, int square);
U64 GenerateKnightAttacks(int square);
U64 GenerateKingAttacks(int square);
void InitTables();
#endif