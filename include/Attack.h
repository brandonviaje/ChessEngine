#ifndef ATTACK_H
#define ATTACK_H

#include "Board.h"
#include "MoveGen.h"
#include "MyTypes.h"
#include "Magic.h"

extern U64 knightAttacks[64];
extern U64 kingAttacks[64];
extern U64 pawnAttacks[2][64];

U64 GeneratePawnAttacks(int side, int square);
U64 GenerateKnightAttacks(int square);
U64 GenerateKingAttacks(int square);
void InitPawnAttacks();
void InitKnightAttacks();
void InitKingAttacks();
U64 GetRookAttacks(int square, U64 blockers);
U64 GetBishopAttacks(int square, U64 blockers);
U64 GetQueenAttacks(int square, U64 blockers);
void InitAttacks();

#endif
