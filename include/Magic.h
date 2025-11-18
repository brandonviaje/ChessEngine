#ifndef MAGIC_H
#define MAGIC_H

#include "Attack.h"
#include "BitBoard.h"
#include "MoveGen.h"
#include "MyTypes.h"

extern U64 attackTable[];

U64 GenerateRookAttacks(int square);
U64 GenerateBishopAttacks(int square);
U64 GenerateQueenAttacks(int square);
U64 GetMagicNumber();
U64 RookMask(int square);
U64 BishopMask(int square);

typedef struct {
    U64 * ptr;
    U64 mask;
    U64 magic;
    int shift;
} SMagic;

#endif