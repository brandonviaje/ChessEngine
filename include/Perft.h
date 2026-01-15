#ifndef PERFT_H
#define PERFT_H

#include "Board.h"
#include "MoveGen.h"
#include "MyTypes.h"

U64 Perft(int depth);
void PerftDivide(int depth);
#endif
