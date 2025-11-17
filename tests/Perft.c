#include "../include/Perft.h"

// Global variables
extern Move moveList[256]; 
extern int moveCount;
extern U64 whitePieces;
extern U64 blackPieces;
extern int side; 

U64 Perft(int depth){
    U64 nodes = 0ULL;

    // base case
    if (depth == 0) 
        return 1ULL;

    GenerateMoves(side == WHITE ? whitePieces : blackPieces, side == WHITE ? blackPieces : whitePieces, side);

    for (int i = 0; i < moveCount; i++) {
        MakeMove(i);
        if (!IsInCheck()){
            nodes += Perft(depth - 1);
        }
        UndoMove(i);
    }

    return nodes;
}

// Perft Divide: print each move and its perft count
void PerftDivide(int depth) {
    printf("Side: %s\n", side == WHITE ? "White" : "Black");
    GenerateMoves(side == WHITE ? whitePieces : blackPieces, side == WHITE ? blackPieces : whitePieces, side);
    U64 totalNodes = 0;

    for(int i = 0; i < moveCount; i++){
        MakeMove(i);
        U64 nodes = Perft(depth - 1);
        UndoMove(i);

        int from = moveList[i].from;
        int to   = moveList[i].to;
        char file_from = 'a' + (from % 8);
        char rank_from = '1' + (from / 8);
        char file_to   = 'a' + (to % 8);
        char rank_to   = '1' + (to / 8);

        printf("%c%c -> %c%c: %llu\n", file_from, rank_from, file_to, rank_to, nodes);
        totalNodes += nodes;
    }

    printf("Nodes searched: %llu\n", totalNodes);
}