#ifndef PERFT_H
#define PERFT_H

#include "MyTypes.h"
#include "Board.h"
#include "MoveGen.h"

U64 Perft(int depth);
void PerftDivide(int depth);
#endif
