#include "Perft.h"

// Global variables
extern Move moveList[256]; 
extern int moveCount;

// Game State
extern int side; 


U64 Perft(int depth){
    U64 nodes = 0ULL;

    // base case
    if (depth == 0) 
        return 1ULL;

    GeneratePseudoLegalMoves(side == WHITE ? whitePieces : blackPieces, side == WHITE ? blackPieces : whitePieces, side);

    for (int i = 0; i < moveCount; i++) {
        MakeMove(i);
        if (IsInCheck() == 0){
            nodes += Perft(depth - 1);
        }
        UndoMove(i);
    }

    return nodes;
}