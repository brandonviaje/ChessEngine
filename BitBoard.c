#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#define starting_position "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define tricky_position "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
typedef unsigned long long U64;

// Create bit board for every piece type and color.
enum {
    P, N, B, R, Q, K,   // white
    p, n, b, r, q, k    // black
};

// Global Variables/ BitBoards

U64 bitboards[12];
U64 whitePieces;     
U64 blackPieces;     
U64 occupied;        
U64 empty;          

int side;
int enpassant;
int castle;

// Initialize the bit boards to 0s
void InitializeBitBoards(){
    
    // Initialize bitboards for each chess piece
    for(int i = 0; i < 12; i++){
        bitboards[i] = 0uLL;
    }

    // Initialize misc. bitboards to 0s
    whitePieces = 0uLL;
    blackPieces = 0uLL;
    occupied = 0uLL;
    empty = 0uLL;
}

int CharToPiece(char c){
    switch(c){
        case 'P': return P;
        case 'N': return N;
        case 'B': return B;
        case 'R': return R;
        case 'Q': return Q;
        case 'K': return K;
        case 'p': return p;
        case 'n': return n;
        case 'b': return b;
        case 'r': return r;
        case 'q': return q;
        case 'k': return k;
        default: return -1; // Did not find
    }
}

// Set Piece on square
void SetPiece(int piece, int square) {
    // set piece on square
    bitboards[piece] |= 1ULL << square;      

    // update white or black bitboard
    if (piece <= K){
        whitePieces |= 1ULL << square;
    }
    else {
        blackPieces |= 1ULL << square;
    }

    // update occupied and empty bitboard
    occupied |= 1ULL << square;
    empty = ~occupied;
}

// Remove piece from square
void RemovePiece(int piece, int square) {\
    // remove piece from square
    bitboards[piece] &= ~(1ULL << square);   

    // update white or black bitboard
    if (piece <= K){
        whitePieces &= ~(1ULL << square);
    } 
    else{
        blackPieces &= ~(1ULL << square);
    } 

    // update occupied and empty bitboard
    occupied &= ~(1ULL << square);
    empty = ~occupied;
}

void PrintBitboard(U64 board) {
    printf("\n");
    for (int rank = 7; rank >= 0; rank--) {  
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            if (board & (1ULL << square))
                printf("1 ");
            else
                printf(". ");
        }
        printf("\n");
    }
    printf("\n");
}

// Parse FEN String
void ParseFEN(char * FEN){
    // Reset the bitboards to update
    InitializeBitBoards();

    char delimiter = '/';
    int rank = 7;
    int file = 0;
    int len = strlen(FEN);

    // When you parse a FEN notation 
    for(int i = 0; i < len; i++){

        // doesn't account for any enpassant, castling check etc.
        if(FEN[i] == ' ') break;

        // Check if you reach a delimiter
        if(FEN[i] == delimiter){
            rank--;
            file = 0;
            continue;
        }
        
        // If you get a number less than 8 skip that many squares
        if(isdigit(FEN[i])){
            file += FEN[i] -'0';
            continue;
        }

        // Update corresponding bitboard
        int piece = CharToPiece(FEN[i]);

        if(piece == -1){
            fprintf(stderr, "Error: Invalid FEN Notation!\n");
            exit(EXIT_FAILURE);
        }

        // Calculate square index of the bitboard
        int square = rank * 8 + file;
        SetPiece(piece,square);

        // Update file
        file++;
    }
    //printf("Successfully PARSED FEN String! \n\n");
}

int main(){
    ParseFEN(starting_position);
    printf("Starting Position of Chessboard: \n");
    PrintBitboard(occupied);
    ParseFEN(tricky_position);
    printf("Tricky Position: \n");
    PrintBitboard(occupied);
    return 0;
}