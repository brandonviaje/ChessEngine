#include "Attack.h"

// Global variables
Move moveList[256]; 
int moveCount = 0;

//BitBoards
extern U64 bitboards[12];
extern U64 blackPieces;
extern U64 whitePieces;
extern U64 occupied;

// Game State
extern int side;
extern int enpassant;  
extern unsigned char castle; 
extern int halfmove;      
extern int fullmove;  

int IsInCheck() {
    // Find king square of the current side
    int kingSquare = __builtin_ctzll(side == WHITE ? bitboards[K] : bitboards[k]);

    // Save current move list, move count and side
    Move savedMoves[256];
    memcpy(savedMoves, moveList, sizeof(moveList));
    int savedMoveCount = moveCount;
    int savedSide = side;

    // Set bitboards according to side
    U64 ownPieces = (side == WHITE ? whitePieces : blackPieces);
    U64 enemyPieces = (side == WHITE? blackPieces : whitePieces);

    //  Generate opponent moves
    side ^= 1; // switch to enemy turn
    GeneratePseudoLegalMoves(enemyPieces, ownPieces, side);
    side = savedSide;  // restore turn

    // Scan for checks 
    for (int i = 0; i < moveCount; i++) {
        if (moveList[i].to == kingSquare) {
            // restore movelist, then return 1
            memcpy(moveList, savedMoves, sizeof(moveList));
            moveCount = savedMoveCount;
            return 1;
        }
    }

    // restore move list
    memcpy(moveList, savedMoves, sizeof(moveList));
    moveCount = savedMoveCount;
    return 0; // no check found
}