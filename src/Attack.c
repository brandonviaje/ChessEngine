#include "Attack.h"

// Global variables
extern Move moveList[256]; 
extern int moveCount;

//BitBoards
extern U64 bitboards[12];
extern U64 blackPieces;
extern U64 whitePieces;
extern U64 occupied;

// Game State
extern int side;

int IsInCheck() {
    // get king square of current side
    int kingSquare = __builtin_ctzll(side == WHITE ? bitboards[K] : bitboards[k]);

    // save current move list and count
    Move savedMoves[256];
    memcpy(savedMoves, moveList, sizeof(moveList));
    int savedMoveCount = moveCount;

    // Generate moves for opponent
    U64 enemyPieces = (side == WHITE ? blackPieces : whitePieces);
    U64 ownPieces   = (side == WHITE ? whitePieces : blackPieces);

    GeneratePseudoLegalMoves(enemyPieces, ownPieces, side ^ 1);

    // Scan moves to see if any attack king square
    for (int i = 0; i < moveCount; i++) {
        if (moveList[i].to == kingSquare) {
            memcpy(moveList, savedMoves, sizeof(moveList));
            moveCount = savedMoveCount;
            return 1; // king is in check
        }
    }

    // restore movelist
    memcpy(moveList, savedMoves, sizeof(moveList));
    moveCount = savedMoveCount;
    return 0; // no check found
}