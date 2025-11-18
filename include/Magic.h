#ifndef MAGIC_H
#define MAGIC_H

#include "Attack.h"
#include "BitBoard.h"
#include "MoveGen.h"
#include "MyTypes.h"

extern U64 attackTable[];

struct SMagic{
    U64 * ptr;
    U64 mask;
    U64 magic;
    int shift;
};

#endif