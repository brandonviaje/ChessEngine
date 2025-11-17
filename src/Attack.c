#include "../include/Attack.h"

//BitBoards
extern U64 bitboards[12];
extern U64 knightMoves[64];
extern U64 kingMoves[64];
extern U64 occupied;

// Game State
extern int side;

int IsInCheck() {
    int kingSquare = __builtin_ctzll(side == WHITE ? bitboards[K] : bitboards[k]);      // get king square of current side
    return IsAttackSquare(side, kingSquare);                                            // check if the king square is getting attacked
}

int IsAttackSquare(int side, int square) {
    U64 target = 1ULL << square;                               // bitmask for the square being checked
    int enemy = (side == WHITE ? BLACK : WHITE);

    // create enemy pieces bitboards
    U64 enemyPawns   = (enemy == WHITE ? bitboards[P] : bitboards[p]);
    U64 enemyKnights = (enemy == WHITE ? bitboards[N] : bitboards[n]);
    U64 enemyBishops = (enemy == WHITE ? bitboards[B] : bitboards[b]);
    U64 enemyRooks   = (enemy == WHITE ? bitboards[R] : bitboards[r]);
    U64 enemyQueens  = (enemy == WHITE ? bitboards[Q] : bitboards[q]);
    U64 enemyKing    = (enemy == WHITE ? bitboards[K] : bitboards[k]);

    // Pawn attacks
    if (enemy == WHITE) {
        if (((enemyPawns << 7) & ~FILE_H) & target) return 1;   // pawn attacks from SW to NE
        if (((enemyPawns << 9) & ~FILE_A) & target) return 1;   // pawn attacks from SE to NW
    } else {
        if (((enemyPawns >> 7) & ~FILE_H) & target) return 1;   // pawn attacks from NW to SE
        if (((enemyPawns >> 9) & ~FILE_A) & target) return 1;   // pawn attacks from NE to SW
    }

    // Knight attacks
    if (knightMoves[square] & enemyKnights) return 1;           // if an enemy knight occupies any of those squares, it can attack the target square

    // King attacks
    if (kingMoves[square] & enemyKing) return 1;                // if the enemy king is in a neighboring square, target is attacked

    // Rook and Queen attacks 
    int rookDirs[4] = { 8, -8, 1, -1 }; 
    for (int i = 0; i < 4; i++) {
        int to = square + rookDirs[i];

        while (to >= 0 && to < 64) {

            // prevent horizontal wrap
            if ((rookDirs[i] == 1 || rookDirs[i] == -1) &&
                (to >> 3) != (square >> 3))
                break;

            U64 mask = 1ULL << to;

            // Stop at first blocking piece
            if (occupied & mask) {
                // check if piece is an enemy rook or queen, target is in check
                if (enemyRooks & mask)  return 1;
                if (enemyQueens & mask) return 1;
                break; // blocked by any piece
            }

            to += rookDirs[i];                          // continue sliding along the ray
        }
    }

    // Bishop and Queen attacks 
    int bishopDirs[4] = { 9, -9, 7, -7 }; 
    for (int i = 0; i < 4; i++) {
        int to = square;
        while (1) {
            int prev = to;
            to += bishopDirs[i];

            if (to < 0 || to > 63) break;

            int prevFile = prev % 8;                  // previous file
            int currFile = to % 8;                    // current file
            if (abs(currFile - prevFile) != 1) break; // prevent diagonal wrap

            U64 mask = 1ULL << to;

            if (occupied & mask) {
                if (enemyBishops & mask) return 1;
                if (enemyQueens  & mask) return 1;
                break;
            }
        }
    }

    return 0;                                           // no enemy pieces attack this square
}