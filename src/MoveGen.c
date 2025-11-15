#include "MoveGen.h"

// Global variables
Move moveList[256]; 
int moveCount = 0;

//BitBoards
extern U64 bitboards[12];
extern U64 blackPieces;
extern U64 whitePieces;
extern U64 occupied;

// Precomputed Table
U64 knightMoves[64];
U64 kingMoves[64];

// Game State
extern int side;
extern int enpassant;  
extern unsigned char castle; 
extern int halfmove;      
extern int fullmove;  

void GeneratePawnMoves(U64 pawns, U64 ownPieces, U64 enemyPieces, int side, int piece){
    if(!pawns) return;
    U64 empty = ~(ownPieces | enemyPieces);

    // generate all single, double, and capture moves for corresponding sides
    U64 singlePush = side == WHITE ? (pawns << 8) & empty : (pawns >> 8) & empty, doublePush;
    U64 leftCapture = side == WHITE ? (pawns << 7) & enemyPieces & ~FILE_H: (pawns >> 9) & enemyPieces & ~FILE_A;
    U64 rightCapture = side == WHITE ? (pawns << 9) & enemyPieces & ~FILE_A : (pawns >> 7) & enemyPieces & ~FILE_H;

    if (side == WHITE) {
        U64 firstStep = (pawns & RANK_2) << 8;
        firstStep &= empty;
        doublePush = (firstStep << 8) & empty;
    } else {
        U64 firstStep = (pawns & RANK_7) >> 8;
        firstStep &= empty;
        doublePush = (firstStep >> 8) & empty;
    }

    // handle en passant
    if (enpassant != -1) {
        U64 epSquare = 1ULL << enpassant;
        U64 epLeft, epRight;

        if (side == WHITE) {
            epLeft  = (pawns << 7) & epSquare & ~FILE_A; // left capture
            epRight = (pawns << 9) & epSquare & ~FILE_H; // right capture
        } else {
            epLeft  = (pawns >> 9) & epSquare & ~FILE_A; // left capture
            epRight = (pawns >> 7) & epSquare & ~FILE_H; // right capture
        }

        // Process EP captures
        while (epLeft) {
            int to = __builtin_ctzll(epLeft);
            int from = side == WHITE ? to - 7 : to + 9;
            int capturedSquare = side == WHITE ? to - 8 : to + 8;
            int capturedPiece = DetectCapture(capturedSquare);
            moveList[moveCount++] = (Move){piece, from, to, -1, capturedPiece, FLAG_ENPASSANT};
            epLeft &= epLeft - 1;
        }

        while (epRight) {
            int to = __builtin_ctzll(epRight);
            int from = side == WHITE ? to - 9 : to + 7;
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

        // promotion check 
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

        // promotion check
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

        // promotion check
        if ((side == WHITE && ((1ULL << to) & RANK_8)) || (side == BLACK && ((1ULL << to) & RANK_1))){
            AddPromotionMoves(from, to, captured, side);
        }else{
            moveList[moveCount++] = (Move){piece, from, to, -1, captured, FLAG_NONE}; // add to move list
        }
        
        rightCapture &= rightCapture - 1; // remove LSB 
    }
}

void GenerateKnightMoves(U64 knights, U64 ownPieces, U64 enemyPieces, int piece) { 
    if(!knights) return;
    U64 knightsCopy = knights; 

    while (knightsCopy) { 
        int from = __builtin_ctzll(knightsCopy); 
        U64 moves = knightMoves[from] & ~ownPieces; // remove squares blocked by own pieces 

        while(moves){ 
            int to = __builtin_ctzll(moves); 
            int captured = DetectCapture(to); 
            moveList[moveCount++] = (Move){piece, from, to, -1, captured, FLAG_NONE}; 
            moves &= moves - 1; // remove LSB 
        } 

        knightsCopy &= knightsCopy - 1; // remove processed knight 
    }  
}

void GenerateKingMoves(U64 king, U64 ownPieces, U64 enemyPieces, int piece) {
    if (!king) return;
    U64 kingsCopy = king;

    while (kingsCopy) {
        int from = __builtin_ctzll(kingsCopy);
        U64 moves = kingMoves[from] & ~ownPieces; // generate all moves at that square

        // Process quiet moves
        while (moves) {
            int to = __builtin_ctzll(moves);
            int captured = DetectCapture(to);
            moveList[moveCount++] = (Move){piece, from, to, -1, captured, FLAG_NONE};
            moves &= moves - 1; // remove LSB
        }

        // Castling 
        if(piece == K){ // white king
            if((castle & (1<<0)) && !(occupied & ((1ULL<<5)|(1ULL<<6)))){ // kingside
                moveList[moveCount++] = (Move){piece, from, 6, -1, -1, FLAG_CASTLE_KINGSIDE};
            }
            
            if((castle & (1<<1)) && !(occupied & ((1ULL<<1)|(1ULL<<2)|(1ULL<<3))) ){ // queenside
                moveList[moveCount++] = (Move){piece, from, 2, -1, -1, FLAG_CASTLE_QUEENSIDE};
            }

        }else if(piece == k){ // black king
            if((castle & (1<<2)) && !(occupied & ((1ULL<<61)|(1ULL<<62))) ){ //kingside
                moveList[moveCount++] = (Move){piece, from, 62, -1, -1, FLAG_CASTLE_KINGSIDE};
            }

            if((castle & (1<<3)) && !(occupied & ((1ULL<<57)|(1ULL<<58)|(1ULL<<59))) ){ // queenside
                moveList[moveCount++] = (Move){piece, from, 58, -1, -1, FLAG_CASTLE_QUEENSIDE};
            }
        }
        kingsCopy &= kingsCopy - 1; // remove processed king
    }
}

void GenerateRookMoves(U64 rooks, U64 ownPieces, U64 enemyPieces,int piece) {
    if(!rooks) return;
    U64 rooksCopy = rooks;

    while (rooksCopy) {
        // get rook position
        int from = __builtin_ctzll(rooksCopy);       
        int directions[4] = {8, -8, 1, -1};         // up, down, right, left

        for (int d = 0; d < 4; d++) {
            int to = from;
            while (1) {
                to += directions[d];                                                            // move one square in direction

                if ((directions[d] == 1 || directions[d] == -1) && (to / 8 != from / 8)) break; // horizontal wrapping
                if (to < 0 || to > 63) break;                                                   // vertical edges
                if (ownPieces & (1ULL << to)) break;                                            // if rook is blocked by own piece, stop sliding

                int captured = DetectCapture(to);                                               // detect captures
                moveList[moveCount++] = (Move){piece, from, to, -1, captured, FLAG_NONE};

                if (enemyPieces & (1ULL << to)) break;                                          // stop sliding after capturing
            }
        }
        rooksCopy &= rooksCopy - 1;          
    }
}

void GenerateBishopMoves(U64 bishops, U64 ownPieces, U64 enemyPieces, int piece) {
    if(!bishops) return;
    U64 bishopsCopy = bishops;

    while (bishopsCopy) {
        int from = __builtin_ctzll(bishopsCopy);
        int directions[4] = {-7, 7, -9, 9}; // diagonals

        for (int d = 0; d < 4; d++) {
            int to = from;

            while (1) {
                int prev = to;
                to += directions[d];  

                if (to < 0 || to > 63) break;                                              // stop if off board or file wrapped

                int prevFile = prev % 8;
                int currFile = to % 8;

                if (abs(currFile - prevFile) != 1) break;                                  // prevent wrapping, difference in file should always be 1
                if (ownPieces & (1ULL << to)) break;                                       // stop if blocked by friendly piece

                int captured = DetectCapture(to);                                          // Detect Captures
                moveList[moveCount++] = (Move){piece, from, to, -1, captured, FLAG_NONE};  // add to move list
        
                if (enemyPieces & (1ULL << to)) break;                                     // stop sliding after capture
            }
        }
        bishopsCopy &= bishopsCopy - 1;
    }
}

void GenerateQueenMoves(U64 queen, U64 ownPieces, U64 enemyPieces, int piece){
    if(!queen) return;
    // Queen moves is the union of rook and bishop generated moves
    GenerateRookMoves(queen, ownPieces, enemyPieces, piece);
    GenerateBishopMoves(queen, ownPieces, enemyPieces, piece);
}

void GenerateMovesInternal(U64 Pawn, U64 Knight, U64 Bishop, U64 Rook, U64 Queen, U64 King, U64 ownPieces, U64 enemyPieces, int side) {
    ResetMoveList();
    GeneratePawnMoves(Pawn, ownPieces, enemyPieces, side, side == WHITE ? P : p);
    GenerateKnightMoves(Knight, ownPieces, enemyPieces, side == WHITE ? N : n);
    GenerateBishopMoves(Bishop, ownPieces, enemyPieces, side == WHITE ? B : b);
    GenerateRookMoves(Rook, ownPieces, enemyPieces, side == WHITE ? R : r);
    GenerateQueenMoves(Queen, ownPieces, enemyPieces, side == WHITE ? Q : q);
    GenerateKingMoves(King, ownPieces, enemyPieces, side == WHITE ? K : k);
}

void GenerateMoves(U64 ownPieces, U64 enemyPieces, int side) {
    if (side == WHITE) {  // white
        GenerateMovesInternal(bitboards[P], bitboards[N], bitboards[B], bitboards[R], bitboards[Q], bitboards[K],ownPieces, enemyPieces, side);
    } else {  // black
        GenerateMovesInternal(bitboards[p], bitboards[n], bitboards[b], bitboards[r], bitboards[q], bitboards[k],ownPieces, enemyPieces, side);
    }
}

// Reset Move List and Move Count
void ResetMoveList(){
    memset(moveList, 0, sizeof(moveList));
    moveCount = 0;
}

void InitKnightMoves(){

    int df[8] = { 2, 2, -2, -2, 1, 1, -1, -1 };
    int dr[8] = { 1, -1, 1, -1, 2, -2, 2, -2 };

    for (int sq = 0; sq < 64; sq++) {
        U64 attacks = 0ULL;
        int file = sq % 8;
        int rank = sq / 8;

        for (int i = 0; i < 8; i++) {
            int nf = file + df[i];
            int nr = rank + dr[i];

            if (nf >= 0 && nf <= 7 && nr >= 0 && nr <= 7) { // check if file and rank are in the board
                int nsq = nr * 8 + nf;
                attacks |= (1ULL << nsq);
            }
        }

        knightMoves[sq] = attacks;
    }
}

void InitKingMoves() {
    for (int square = 0; square < 64; square++) {
        U64 bb = 1ULL << square;
        U64 moves = 0ULL;

        // Possible king moves
        moves |= (bb << 8);                     // N
        moves |= (bb >> 8);                     // S
        moves |= (bb << 1) & ~FILE_A;           // E
        moves |= (bb >> 1) & FILE_H;            // W
        moves |= (bb << 9) & ~FILE_A;           // NE
        moves |= (bb << 7) & ~FILE_H;           // NW
        moves |= (bb >> 9) & ~FILE_H;           // SW
        moves |= (bb >> 7) & ~FILE_A;           // SE

        kingMoves[square] = moves;              // add possible moves at that square
    }
}

int DetectCapture(int to) {
    // white side: start at black piece bb black side: start at white piece bb
    int start = (side == WHITE) ? 6 : 0;
    int end   = (side == WHITE) ? 12 : 6;

    U64 target = 1ULL << to;

    for (int i = start; i < end; i++) {
        if (bitboards[i] & target)
            return i; // captured piece index
    }

    return -1; // no capture
}

// Add promotion moves for pawns
void AddPromotionMoves(int from, int to, int captured, int side) {
    if(side == WHITE){
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

// Initialize precomputed move table
void InitTables(){
    InitKnightMoves();
    InitKingMoves();
}

int main(){
    InitTables();
    ParseFEN(starting_position); 
    for(int i = 0 ; i <=5;i++){
        U64 nodes = Perft(i);
        printf("Nodes at depth %d: %llu \n", i, nodes);
    }
    return 0;
}