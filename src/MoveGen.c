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
    U64 singlePush = side == WHITE ? (pawns << 8) & empty : (pawns >> 8) & empty;
    U64 doublePush = 0;

    // Generate capture moves: left and right capture for corresponding sides
    U64 leftCapture = side == WHITE ? (pawns << 7) & enemyPieces : (pawns >> 7) & enemyPieces;
    U64 rightCapture = side == WHITE ? (pawns << 9) & enemyPieces : (pawns >> 9) & enemyPieces;
    
    if(side == WHITE) {
        U64 firstPushWhite = ((pawns & RANK_2) << 8) & empty; 
        doublePush = (firstPushWhite << 8) & empty;        
    } else {
        U64 firstPushBlack = ((pawns & RANK_7) >> 8) & empty;
        doublePush = (firstPushBlack >> 8) & empty;
    }

    // Handle en passant
    if (enpassant != -1) {
        U64 epSquare = 1ULL << enpassant;
        U64 epLeft, epRight;

        if (side == WHITE) {
            // white pawns that can capture en passant
            epLeft  = (pawns << 7) & epSquare & ~FILE_H; 
            epRight = (pawns << 9) & epSquare & ~FILE_A;
        } else {
            // black pawns that can capture en passant
            epLeft  = (pawns >> 9) & epSquare & ~FILE_H; 
            epRight = (pawns >> 7) & epSquare & ~FILE_A;
        }

        // Process EP captures

        while (epLeft) {
            int from = side == WHITE ? __builtin_ctzll(epLeft) - 7 : __builtin_ctzll(epLeft) + 9;
            int to = enpassant;
            int capturedSquare = side == WHITE ? to - 8 : to + 8;
            int capturedPiece = DetectCapture(capturedSquare);
            moveList[moveCount++] = (Move){piece, from, to, -1, capturedPiece, FLAG_ENPASSANT};
            epLeft &= epLeft - 1;
        }

        while (epRight) {
            int from = side == WHITE ? __builtin_ctzll(epRight) - 9 : __builtin_ctzll(epRight) + 7;
            int to = enpassant;
            int capturedSquare = side == WHITE ? to - 8 : to + 8;
            int capturedPiece = DetectCapture(capturedSquare);
            moveList[moveCount++] = (Move){piece, from, to, -1, capturedPiece, FLAG_ENPASSANT};
            epRight &= epRight - 1;
        }
    }

    // Process quiet and capture moves
    while(singlePush){
        int to = __builtin_ctzll(singlePush);
        int from = side == WHITE ? to - 8 : to + 8;

        // promotion check : if white pawn is on rank 8 or black pawn is on rank 1, promotion is possible
        if ((side == WHITE && ((1ULL << to) & RANK_8)) || (side == BLACK && ((1ULL << to) & RANK_1))){
            AddPromotionMoves(from, to, -1, side);
        }else{
            moveList[moveCount++] = (Move){piece, from, to, -1, -1, FLAG_NONE}; // add to move list
        }

        singlePush &= singlePush - 1; // remove LSB
    }

    while(doublePush){
        int to = __builtin_ctzll(doublePush);
        int from = side == WHITE ? to - 16 : to + 16;
        moveList[moveCount++] = (Move){piece, from, to, -1, -1, FLAG_NONE}; // add to move list
        doublePush &= doublePush - 1; // remove LSB
    }

    while(leftCapture){
        int to = __builtin_ctzll(leftCapture);
        int from = side == WHITE ? to - 7 : to + 9;
        int captured = DetectCapture(to);

        // promotion check : if white pawn is on rank 8 or black pawn is on rank 1, promotion is possible
        if ((side == WHITE && ((1ULL << to) & RANK_8)) || (side == BLACK && ((1ULL << to) & RANK_1))){
            AddPromotionMoves(from, to, captured, side);
        }else{
            moveList[moveCount++] = (Move){piece, from, to, -1, captured, FLAG_NONE}; // add to move list
        }

        leftCapture &= leftCapture - 1; // remove LSB
    }

    while(rightCapture){
        int to = __builtin_ctzll(rightCapture);
        int from = side == WHITE ? to - 9 : to + 7;
        int captured = DetectCapture(to);

        // promotion check : if white pawn is on rank 8 or black pawn is on rank 1, promotion is possible
        if ((side == WHITE && ((1ULL << to) & RANK_8)) || (side == BLACK && ((1ULL << to) & RANK_1))){
            AddPromotionMoves(from, to, captured, side);
        }else{
            moveList[moveCount++] = (Move){piece, from, to, -1, captured, FLAG_NONE}; // add to move list
        }
        
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
            moveList[moveCount++] = (Move){piece, from, to, -1, captured, FLAG_NONE};
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
            moveList[moveCount++] = (Move){piece, from, to, -1, captured, FLAG_NONE}; 
        }
        // remove LSB of kings copy
        kingsCopy &= kingsCopy - 1;
    }

    // Castling 
    if(piece == K){ // white king
        int from = __builtin_ctzll(king);

        // white kingside: check if castling is available by checking empty squares
        if((castle & (1<<0)) && !(occupied & ((1ULL<<5)|(1ULL<<6))) /* squares between empty */){
            moveList[moveCount++] = (Move){piece, from, 6, -1, -1, FLAG_CASTLE_KINGSIDE};
        }

        // white queenside: check if castling is available by checking empty squares
        if((castle & (1<<1)) && !(occupied & ((1ULL<<1)|(1ULL<<2)|(1ULL<<3))) ){
            moveList[moveCount++] = (Move){piece, from, 2, -1, -1, FLAG_CASTLE_QUEENSIDE};
        }
    }else if(piece == k){ // black king
        int from = __builtin_ctzll(king);

        // black kingside: check if castling is available by checking empty squares
        if((castle & (1<<2)) && !(occupied & ((1ULL<<61)|(1ULL<<62))) ){
            moveList[moveCount++] = (Move){piece, from, 62, -1, -1, FLAG_CASTLE_KINGSIDE};
        }

        // black queenside: check if castling is available by checking empty squares
        if((castle & (1<<3)) && !(occupied & ((1ULL<<57)|(1ULL<<58)|(1ULL<<59))) ){
            moveList[moveCount++] = (Move){piece, from, 58, -1, -1, FLAG_CASTLE_QUEENSIDE};
        }
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
                moveList[moveCount++] = (Move){piece, from, to, -1, captured, FLAG_NONE};

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
                moveList[moveCount++] = (Move){piece, from, to, -1, captured, FLAG_NONE};

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
    GeneratePawnMoves(Pawn, ownPieces, enemyPieces, side, side == WHITE ? P : p);
    GenerateKnightMoves(Knight, ownPieces, enemyPieces, side == WHITE ? N : n);
    GenerateBishopMoves(Bishop, ownPieces, enemyPieces, side == WHITE ? B : b);
    GenerateRookMoves(Rook, ownPieces, enemyPieces, side == WHITE ? R : r);
    GenerateQueenMoves(Queen, ownPieces, enemyPieces, side == WHITE ? Q : q);
    GenerateKingMoves(King, ownPieces, enemyPieces, side == WHITE ? K : k);
}

void GeneratePseudoLegalMoves(U64 ownPieces, U64 enemyPieces, int side) {
    if (side == WHITE) {  // white
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
    for (int i = (side == WHITE ? 6 : 0); i < (side == WHITE ? 12 : 6); i++){
        if (bitboards[i] & (1ULL << to)){
            captured = i;
            break;
        }
    }
    return captured;
}

// Add promotion moves for pawns
void AddPromotionMoves(int from, int to, int captured, int side) {
    if(side == 0){
        moveList[moveCount++] = (Move){P, from, to, Q, captured, FLAG_PROMOTION};
        moveList[moveCount++] = (Move){P, from, to, R, captured, FLAG_PROMOTION};
        moveList[moveCount++] = (Move){P, from, to, B, captured, FLAG_PROMOTION};
        moveList[moveCount++] = (Move){P, from, to, N, captured, FLAG_PROMOTION};
    } else {
        moveList[moveCount++] = (Move){p, from, to, q, captured, FLAG_PROMOTION};
        moveList[moveCount++] = (Move){p, from, to, r, captured, FLAG_PROMOTION};
        moveList[moveCount++] = (Move){p, from, to, b, captured, FLAG_PROMOTION};
        moveList[moveCount++] = (Move){p, from, to, n, captured, FLAG_PROMOTION};
    }
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
    ParseFEN(castling_position); 
    PrintBitboard(occupied);
    GeneratePseudoLegalMoves(side == 0 ? whitePieces : blackPieces, side == WHITE ? blackPieces : whitePieces, side);
    PrintMoveList();
    MakeMove(24);
    PrintBitboard(occupied);
    UndoMove(24);
    PrintBitboard(occupied);
    return 0;
}