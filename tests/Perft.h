#ifndef PERFT_H
#define PERFT_H

#include "../src/BitBoard.h"
#include "../src/Attack.h"
#include "../src/MoveGen.h"
#include "../src/MyTypes.h"

extern Move moveList[256]; 
extern int moveCount;
extern U64 whitePieces;
extern U64 blackPieces;
extern int side; 

U64 Perft(int depth);

#endif