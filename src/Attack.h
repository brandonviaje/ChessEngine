#ifndef ATTACK_H
#define ATTACK_H

#include "BitBoard.h"
#include "MoveGen.h"
#include "MyTypes.h"

extern Move moveList[256]; 
extern int moveCount;
extern U64 bitboards[12];
extern U64 blackPieces;
extern U64 whitePieces;
extern int side;

int IsInCheck();

#endif