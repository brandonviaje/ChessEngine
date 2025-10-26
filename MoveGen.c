#include <stdio.h>
#include "MoveGen.h"

// Global variables
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

void GenerateRookMoves(U64 rooks, U64 ownPieces, U64 enemyPieces) {
    U64 rooksCopy = rooks;

    while (rooksCopy) {
        // get rook position
        int from = __builtin_ctzll(rooksCopy);    
        rooksCopy &= rooksCopy - 1;             
        int directions[4] = {8, -8, 1, -1};         // up, down, right, left

        for (int d = 0; d < 4; d++) {

            int i = from;

            while (1) {
                // move one square in direction
                i += directions[d];

                // horizontal wrapping
                if ((directions[d] == 1 || directions[d] == -1) && (i / 8 != from / 8)) break;

                // vertical edges
                if (i < 0 || i > 63) break;

                // add move to move list
                moveList[moveCount++] = (Move){from, i, 0};

                // stop sliding if blocked
                if ((ownPieces | enemyPieces) & (1ULL << i)) break;
            }
        }
    }
}

void GenerateBishopMoves(U64 bishops, U64 ownPieces, U64 enemyPieces) {
    U64 bishopsCopy = bishops;

    while (bishopsCopy) {
        // Get bishop position
        int from = __builtin_ctzll(bishopsCopy);
        bishopsCopy &= bishopsCopy - 1;

        int directions[4] = {-7, 7, -9, 9}; // diagonals

        for (int d = 0; d < 4; d++) {
            
            int i = from;

            while (1) {

                // Check file edge for diagonal wrapping
                int prevFile = i % 8;
                i += directions[d];
                int currFile = i % 8;

                if (i < 0 || i > 63 || abs(currFile - prevFile) != 1) break;   // vertical edges or horizontal wrap
                moveList[moveCount++] = (Move){from, i, 0};                    // Add move to move list
                if ((ownPieces | enemyPieces) & (1ULL << i)) break;            // Stop sliding if blocked
            }
        }
    }
}

void GenerateQueenMoves(U64 queen, U64 ownPieces, U64 enemyPieces){
    // Queen moves is the union of rook and bishop generated moves
    GenerateRookMoves(queen, ownPieces, enemyPieces);
    GenerateBishopMoves(queen, ownPieces, enemyPieces);
}

void ResetMoveList(){
    // Reset move list array and move count
    memset(moveList, 0, sizeof(moveList));
    moveCount = 0;
}

void GenerateAllMoves(U64 P, U64 N, U64 B, U64 R, U64 Q, U64 K, U64 ownPieces, U64 enemyPieces, int side) {
    // Reset move list, generate all pseudo moves for each piece type
    ResetMoveList();
    GeneratePawnMoves(P, ownPieces, enemyPieces, side);
    GenerateKnightMoves(N, ownPieces, enemyPieces);
    GenerateBishopMoves(B, ownPieces, enemyPieces);
    GenerateRookMoves(R, ownPieces, enemyPieces);
    GenerateQueenMoves(Q, ownPieces, enemyPieces);
    GenerateKingMoves(K, ownPieces, enemyPieces);
}

int main(){
    return 0;
}