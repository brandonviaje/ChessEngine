#ifndef PERFT_H
#define PERFT_H

#include "BitBoard.h"
#include "Attack.h"
#include "MoveGen.h"
#include "MyTypes.h"

extern Move moveList[256]; 
extern int moveCount;
extern U64 whitePieces;
extern U64 blackPieces;
extern int side; 

U64 Perft(int depth);
void PerftDivide(int depth);
#endif