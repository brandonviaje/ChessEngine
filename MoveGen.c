#include <stdio.h>
#include "MoveGen.h"

Move moveList[256]; 
int moveCount = 0;

void GeneratePawnMoves(U64 pawns, U64 ownPieces, U64 enemyPieces, int side){

    // Generate all single & double push moves for corresponding sides
    U64 singlePush = side == 0 ? (pawns & rank2) << 8 & empty : (pawns & rank7) >> 8 & empty;
    U64 doublePush = side == 0 ? (singlePush << 8) & empty & rank4 : (singlePush >> 8) & empty & rank5;

    while(singlePush){
        int to = __builtin_ctzll(singlePush);
        int from = side == 0 ? to - 8 : to + 8;
        moveList[moveCount++] = (Move){from,to,0}; // add to move list
        singlePush &= singlePush - 1; // remove LSB
    }

    while(doublePush){
        int to = __builtin_ctzll(doublePush);
        int from = side == 0 ? to - 16 : to + 16;
        moveList[moveCount++] = (Move){from,to,0}; // add to move list
        doublePush &= doublePush - 1; // remove LSB
    }
}

void GenerateKnightMoves(U64 knights, U64 ownPieces, U64 enemyPieces){
    
    // Make a copy so we don’t destroy the original bitboard
    U64 knightsCopy = knights; 

    while(knightsCopy){

        // Get current knight’s square
        int from = __builtin_ctzll(knightsCopy); 

        // All possible moves
        U64 possibleMoves = 0;
        possibleMoves |= 1ULL << (from + 17); // up 2, right 1
        possibleMoves |= 1ULL << (from + 15); // up 2, left 1
        possibleMoves |= 1ULL << (from + 10); // up 1, right 2
        possibleMoves |= 1ULL << (from + 6);  // up 1, left 2
        possibleMoves |= 1ULL << (from - 17); // down 2, left 1
        possibleMoves |= 1ULL << (from - 15); // down 2, right 1
        possibleMoves |= 1ULL << (from - 10); // down 1, left 2
        possibleMoves |= 1ULL << (from - 6);  // down 1, right 2

        while(possibleMoves){
            int to = __builtin_ctzll(possibleMoves); // destination square
            moveList[moveCount++] = (Move){from, to, 0};
            possibleMoves &= possibleMoves - 1; // remove this destination bit
        }

        knightsCopy &= knightsCopy - 1; // remove processed knight
    }
}

void GenerateKingMoves(U64 king, U64 ownPieces, U64 enemyPieces){
    // Make a copy so we don’t destroy the original bitboard
    U64 kingsCopy = king;

    while(kingsCopy){
        // Get the king's position
        int from = __builtin_ctzll(kingsCopy);
        U64 possibleMoves = 0;

        // Generate all kings possible moves
        possibleMoves |= 1uLL << (from + 9);
        possibleMoves |= 1uLL << (from + 8);
        possibleMoves |= 1uLL << (from + 7);
        possibleMoves |= 1uLL << (from + 1);
        possibleMoves |= 1uLL << (from - 9);
        possibleMoves |= 1uLL << (from - 8);
        possibleMoves |= 1uLL << (from - 7);
        possibleMoves |= 1uLL << (from - 1);

        while (possibleMoves){
            // Get the LSB's possible move
            int to = __builtin_ctzll(possibleMoves); 
            moveList[moveCount++] = (Move){from, to, 0};
            possibleMoves &= possibleMoves - 1; // remove least significant bit
        }
        // remove LSB of kings copy
        kingsCopy &= kingsCopy - 1;
    }
}

void GenerateRookMoves(U64 rooks, U64 ownPieces, U64 enemyPieces){

}

void GenerateBishopMoves(U64 bishops, U64 ownPieces, U64 enemyPieces){

}

void GenerateQueenMoves(U64 queen, U64 ownPieces, U64 enemyPieces){

}

void ResetMoveList(){
    // Reset move list array and move count
    memset(moveList, 0, sizeof(moveList));
    moveCount = 0;
}

int main(){
    return 0;
}