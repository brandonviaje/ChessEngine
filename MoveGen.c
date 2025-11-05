#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "MoveGen.h"

// Global variables
Move moveList[256]; 
int moveCount = 0;
extern U64 bitboards[12];
extern U64 blackPieces;
extern U64 whitePieces;
extern U64 occupied;

void GeneratePawnMoves(U64 pawns, U64 ownPieces, U64 enemyPieces, int side){
    U64 empty = ~(ownPieces | enemyPieces);

    // Generate all single & double push moves for corresponding sides
    U64 singlePush = side == 0 ? (pawns << 8) & empty : (pawns >> 8) & empty;
    U64 doublePush = 0;
    
    if(side == 0) {
        U64 firstPushWhite = (pawns & RANK_2) << 8 & empty; 
        doublePush = (firstPushWhite << 8) & empty;        
    } else {
        U64 firstPushBlack = (pawns & RANK_7) >> 8 & empty;
        doublePush = (firstPushBlack >> 8) & empty;
    }

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
    U64 empty = ~(ownPieces | enemyPieces);

    while(knightsCopy){

        // Get current knight’s square
        int from = __builtin_ctzll(knightsCopy); 

        // All possible moves
        U64 possibleMoves = 0;

        // Define knight move offsets and edge masks
        const int offsets[8] = {17, 15, 10, 6, -17, -15, -10, -6};
        const U64 unsafeFiles[8] = {
            FILE_H, FILE_A, FILE_G | FILE_H, FILE_A | FILE_B,
            FILE_A, FILE_H, FILE_A | FILE_B, FILE_G | FILE_H
        };
        const U64 unsafeRanks[8] = {
            RANK_7 | RANK_8, RANK_7 | RANK_8, RANK_8, RANK_8,
            RANK_1 | RANK_2, RANK_1 | RANK_2, RANK_1, RANK_1
        };
        
        for (int i = 0; i < 8; i++) {
            int to = from + offsets[i];

            // Skip if knight is on an unsafe file/rank
            if ((1ULL << from & unsafeFiles[i]) || (1ULL << from & unsafeRanks[i]))
                continue;

            // Skip if move goes off the board
            if (to < 0 || to > 63)
                continue;

            possibleMoves |= 1ULL << to & empty;
        }

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
    U64 empty = ~(ownPieces | enemyPieces);

    while(kingsCopy){
        // Get the king's position
        int from = __builtin_ctzll(kingsCopy);
        U64 possibleMoves = 0;

        // King move offsets and edge masking
        const int offsets[8] = {9, 8, 7, 1, -1, -7, -8, -9};
        const U64 unsafeFiles[8] = {
            FILE_H, 0, FILE_A, FILE_H, FILE_A, FILE_H, 0, FILE_A
        };
        const U64 unsafeRanks[8] = {
            RANK_8, RANK_8, RANK_8, 0, 0, RANK_1, RANK_1, RANK_1
        };

        for (int i = 0; i < 8; i++) {
            int to = from + offsets[i];

            // Skip if king is on an unsafe file/rank
            if ((1ULL << from & unsafeFiles[i]) || (1ULL << from & unsafeRanks[i]))
                continue;

            // Skip if move goes off the board
            if (to < 0 || to > 63)
                continue;

            possibleMoves |= 1ULL << to & empty;
        }

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

                // if rook is blocked by own piece, stop sliding
                if (ownPieces & (1ULL << i)) break;  

                // add move to move list
                moveList[moveCount++] = (Move){from, i, 0};

                // stop sliding after capturing
                if (enemyPieces & (1ULL << i)) break;
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

                // vertical edges or horizontal wrap
                if (i < 0 || i > 63 || abs(currFile - prevFile) != 1) break;   

                // if bishop blocked by own piece, stop sliding
                if(ownPieces & 1ULL << i) break;

                moveList[moveCount++] = (Move){from, i, 0};                    // Add move to move list

                // Stop sliding after capturing
                if (enemyPieces & (1ULL << i)) break;            
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

void printMoveList(){
    printf("Move List Count: %d\n", moveCount);
    for(int i = 0; i < moveCount; i++){
        int from = moveList[i].from;
        int to   = moveList[i].to;
        char file_from = 'a' + (from % 8);
        char rank_from = '1' + (from / 8);
        char file_to   = 'a' + (to % 8);
        char rank_to   = '1' + (to / 8);

        printf("Move %d: %c%c -> %c%c\n", i+1, file_from, rank_from, file_to, rank_to);
    }
}

int main(){
    ParseFEN(tricky_position); 
    PrintBitboard(whitePieces);
    GenerateAllMoves(bitboards[P],bitboards[N],bitboards[B],bitboards[R],bitboards[Q],bitboards[K],whitePieces,blackPieces,0);
    printMoveList();
    return 0;
}