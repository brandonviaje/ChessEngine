#ifndef ATTACK_H
#define ATTACK_H

#include "BitBoard.h"
#include "MoveGen.h"
#include "MyTypes.h"

int IsInCheck();
int IsAttackSquare(int side, int square);

#endif