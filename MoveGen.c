#include "MoveGen.h"

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

void GeneratePawnMoves(U64 pawns, U64 ownPieces, U64 enemyPieces, int side, int piece){
    U64 empty = ~(ownPieces | enemyPieces);

    // Generate all single & double push moves for corresponding sides
    U64 singlePush = side == 0 ? (pawns << 8) & empty : (pawns >> 8) & empty;
    U64 doublePush = 0;

    // Generate capture moves: left and right capture for corresponding sides
    U64 leftCapture = side == 0 ? (pawns << 7) & enemyPieces : (pawns >> 7) & enemyPieces;
    U64 rightCapture = side == 0 ? (pawns << 9) & enemyPieces : (pawns >> 9) & enemyPieces;
    
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
        moveList[moveCount++] = (Move){piece, from, to, 0, -1};// add to move list
        singlePush &= singlePush - 1; // remove LSB
    }

    while(doublePush){
        int to = __builtin_ctzll(doublePush);
        int from = side == 0 ? to - 16 : to + 16;
        moveList[moveCount++] = (Move){piece, from, to, 0, -1}; // add to move list
        doublePush &= doublePush - 1; // remove LSB
    }

    while(leftCapture){
        int to = __builtin_ctzll(leftCapture);
        int from = side == 0 ? to - 7 : to + 9;

        // Detect Captures
        int captured = DetectCapture(to);

        // Add to move list
        moveList[moveCount++] = (Move){piece, from, to, 0, captured};
        leftCapture &= leftCapture - 1; // remove LSB
    }

    while(rightCapture){
        int to = __builtin_ctzll(rightCapture);
        int from = side == 0 ? to - 9 : to + 7;

        // Detect Captures
        int captured = DetectCapture(to);
        // Add to move list
        moveList[moveCount++] = (Move){piece, from, to, 0, captured};
        rightCapture &= rightCapture - 1; // remove LSB (done processing this piece)
    }
}

void GenerateKnightMoves(U64 knights, U64 ownPieces, U64 enemyPieces, int piece){
    
    // Make a copy so we don’t destroy the original bitboard
    U64 knightsCopy = knights; 

    while(knightsCopy){

        // Get current knight’s square
        int from = __builtin_ctzll(knightsCopy); 

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

            // Skip if move goes off the board
            if ((1ULL << from & unsafeFiles[i]) || (1ULL << from & unsafeRanks[i])) continue;
            if (to < 0 || to > 63) continue;
            if(ownPieces & (1ULL << to)) continue;

            // Detect Captures
            int captured = DetectCapture(to);

            //Add to move list
            moveList[moveCount++] = (Move){piece, from, to, 0, captured};
        }
        knightsCopy &= knightsCopy - 1; // remove processed knight
    }
}

void GenerateKingMoves(U64 king, U64 ownPieces, U64 enemyPieces, int piece){
    // Make a copy so we don’t destroy the original bitboard
    U64 kingsCopy = king;

    while(kingsCopy){
        // Get the king's position
        int from = __builtin_ctzll(kingsCopy);

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

            // Skip if move goes off the board
            if ((1ULL << from & unsafeFiles[i]) || (1ULL << from & unsafeRanks[i])) continue;            
            if (to < 0 || to > 63) continue;
            if (ownPieces & (1ULL << to)) continue;

            // Detect Captures
            int captured = DetectCapture(to);

            // Add to move list
            moveList[moveCount++] = (Move){piece, from, to, 0 , captured}; 
        }
        // remove LSB of kings copy
        kingsCopy &= kingsCopy - 1;
    }
}

void GenerateRookMoves(U64 rooks, U64 ownPieces, U64 enemyPieces,int piece) {
    U64 rooksCopy = rooks;

    while (rooksCopy) {
        // get rook position
        int from = __builtin_ctzll(rooksCopy);       
        int directions[4] = {8, -8, 1, -1};         // up, down, right, left

        for (int d = 0; d < 4; d++) {
            int to = from;
            while (1) {
                // move one square in direction
                to += directions[d];

                // horizontal wrapping
                if ((directions[d] == 1 || directions[d] == -1) && (to / 8 != from / 8)) break;

                // vertical edges
                if (to < 0 || to > 63) break;

                // if rook is blocked by own piece, stop sliding
                if (ownPieces & (1ULL << to)) break;  

                // Detect captures
                int captured = DetectCapture(to);

                // Add to move list
                moveList[moveCount++] = (Move){piece, from, to, 0, captured};

                // stop sliding after capturing
                if (enemyPieces & (1ULL << to)) break;
            }
        }
        rooksCopy &= rooksCopy - 1;          
    }
}

void GenerateBishopMoves(U64 bishops, U64 ownPieces, U64 enemyPieces, int piece) {
    U64 bishopsCopy = bishops;

    while (bishopsCopy) {
        int from = __builtin_ctzll(bishopsCopy);
        int directions[4] = {-7, 7, -9, 9}; // diagonals

        for (int d = 0; d < 4; d++) {
            int to = from;

            while (1) {
                int prev = to;
                to += directions[d];  

                // stop if off board or file wrapped
                if (to < 0 || to > 63) break;

                int prevFile = prev % 8;
                int currFile = to % 8;

                if (abs(currFile - prevFile) != 1) break;

                // stop if blocked by friendly piece
                if (ownPieces & (1ULL << to)) break;

                // Detect Captures
                int captured = DetectCapture(to);

                // Add to move list
                moveList[moveCount++] = (Move){piece, from, to, 0, captured};

                // stop sliding after capture
                if (enemyPieces & (1ULL << to)) break;
            }
        }
        bishopsCopy &= bishopsCopy - 1;
    }
}

void GenerateQueenMoves(U64 queen, U64 ownPieces, U64 enemyPieces, int piece){
    // Queen moves is the union of rook and bishop generated moves
    GenerateRookMoves(queen, ownPieces, enemyPieces, piece);
    GenerateBishopMoves(queen, ownPieces, enemyPieces, piece);
}

void GeneratePseudoLegalMovesInternal(U64 Pawn, U64 Knight, U64 Bishop, U64 Rook, U64 Queen, U64 King, U64 ownPieces, U64 enemyPieces, int side) {
    ResetMoveList();
    GeneratePawnMoves(Pawn, ownPieces, enemyPieces, side, side == 0 ? P : p);
    GenerateKnightMoves(Knight, ownPieces, enemyPieces, side == 0 ? N : n);
    GenerateBishopMoves(Bishop, ownPieces, enemyPieces, side == 0 ? B : b);
    GenerateRookMoves(Rook, ownPieces, enemyPieces, side == 0 ? R : r);
    GenerateQueenMoves(Queen, ownPieces, enemyPieces, side == 0 ? Q : q);
    GenerateKingMoves(King, ownPieces, enemyPieces, side == 0 ? K : k);
}

void GeneratePseudoLegalMoves(U64 ownPieces, U64 enemyPieces, int side) {
    if (side == 0) {  // white
        GeneratePseudoLegalMovesInternal(bitboards[P], bitboards[N], bitboards[B], bitboards[R], bitboards[Q], bitboards[K],ownPieces, enemyPieces, side);
    } else {  // black
        GeneratePseudoLegalMovesInternal(bitboards[p], bitboards[n], bitboards[b], bitboards[r], bitboards[q], bitboards[k],ownPieces, enemyPieces, side);
    }
}

/**
 * 
 * 
 * HELPER FUNCTIONS
 * 
 * 
 */

// Reset Move List
void ResetMoveList(){
    // Reset move list array and moveCount
    memset(moveList, 0, sizeof(moveList));
    moveCount = 0;
}

int DetectCapture(int to) {
    // -1 If no capture
    int captured = -1;
    
    // Detect Captures on the Enemy side bitboards
    for (int i = (side == 0 ? 6 : 0); i < (side == 0 ? 12 : 6); i++){
        if (bitboards[i] & (1ULL << to)){
            captured = i;
            break;
        }
    }
    return captured;
}

// Make Move Function
void MakeMove(int index){

    if (index < 0 || index > MAX_MOVES || index > moveCount){
        fprintf(stderr, "%s", "Error: Invalid Move Index\n");
        exit(EXIT_FAILURE);
    }

    int piece = moveList[index].piece;
    int from = moveList[index].from;
    int to = moveList[index].to;

    if (from < 0 || from >= 64 || to < 0 || to >= 64) {
        fprintf(stderr, "Error: Invalid square index (%d -> %d)\n", from, to);
        exit(EXIT_FAILURE);
    }

    int fromMask = 1ULL << from;
    int toMask = 1ULL << to;
    
    bitboards[piece] ^= fromMask; // remove piece using fromMask
    bitboards[piece] ^= toMask;  // add piece using toMask

    // Update the corresponding color bitboard
    if (piece <= K){
        whitePieces ^= fromMask;
        whitePieces ^= toMask;
    }else{
        blackPieces ^= fromMask;
        blackPieces ^= toMask;
    }

    // Update occupied bitboard
    occupied ^= fromMask;
    occupied ^= toMask;
}

// Make Undo Move function
void UndoMove(int index){

    if (index < 0 || index > MAX_MOVES || index > moveCount){
        fprintf(stderr, "%s", "Error: Invalid Move Index\n");
        exit(EXIT_FAILURE);
    }

    int piece = moveList[index].piece;
    // Previous was our from 
    int prev = moveList[index].from;
    // Current square piece is on
    int current = moveList[index].to;

    if (prev < 0 || prev >= 64 || current < 0 || current >= 64) {
        fprintf(stderr, "Error: Invalid square index (%d -> %d)\n", prev, current);
        exit(EXIT_FAILURE);
    }

    // Create prevMask and currentMask
    int prevMask = 1ULL << prev;
    int currentMask = 1ULL << current;

    // remove piece using currentMask
    bitboards[piece] ^= currentMask;
    // undo move using prevMask
    bitboards[piece] ^= prevMask;

    // Update the corresponding color bitboard
    if (piece <= K){
        whitePieces ^= currentMask;
        whitePieces ^= prevMask;
    }else{
        blackPieces ^= currentMask;
        blackPieces ^= prevMask;
    }

    // Update occupied bitboard
    occupied ^= currentMask;
    occupied ^= prevMask;
}

void PrintMoveList(){
    printf("Move List Count: %d\n\n", moveCount);
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
    ParseFEN(queen_attack_position); 
    PrintBitboard(occupied);
    GeneratePseudoLegalMoves(side == 0 ? whitePieces : blackPieces, side == 0 ? blackPieces : whitePieces, side);
    PrintMoveList();
    return 0;
}

/*
 * 
 * Make Move Does not account for capture moves, en passant, castling, etc.
 * Undo Move Does not account for capture moves, en passant, castling, etc.
 * Need to build legal moves by checking if king is in check
 * Implement isInCheck() function later.
 * 
 */