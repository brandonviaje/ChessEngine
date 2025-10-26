#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include "BitBoard.h"

// Bitboards

U64 bitboards[12];
U64 whitePieces;     
U64 blackPieces;     
U64 occupied;        

// Game states

int side; // 0 = white, 1 = black
int enpassant;
unsigned char castle;
int halfmove;
int fullmove;

// Initialize  bitboards to 0s
void ResetBoardState(){

    // Reset game state variables
    side = -1;
    enpassant = -1;
    castle = 0; // set default to no castle rights (if it is a '-')
    halfmove = 0;
    fullmove = 0;
    
    // Initialize bitboards for each chess piece
    for(int i = 0; i < 12; i++){
        bitboards[i] = 0uLL;
    }

    // Initialize misc. bitboards to 0s
    whitePieces = 0uLL;
    blackPieces = 0uLL;
    occupied = 0uLL;
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
}

// Remove piece from square
void RemovePiece(int piece, int square) {
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

    // Print game state
    PrintGameState();
}

void PrintGameState(){
    // Display Current Side
    side == 0 ? printf("Current Turn: White \n\n") : printf("Current Turn: Black \n\n");

    // Display Enpassant Index
    enpassant == -1 ? printf("No valid enpassant \n") : printf("En passant at index: %d \n", enpassant);

    // Display castle status
    if(castle == 0) printf("No castle");
    printf("Castling rights: ");
    if(castle & 1)  printf("K");  // White kingside
    if(castle & 2)  printf("Q");  // White queenside
    if(castle & 4)  printf("k");  // Black kingside
    if(castle & 8)  printf("q");  // Black queenside
    printf("\n");
}

// Parse FEN String to capture the current game state
void ParseFEN(char * FEN){

    // Parse FEN into 6 fields
    char *fields[6];
    int i = 0;
    char *fen_copy = strdup(FEN);
    char *token = strtok(fen_copy, " ");

    while(token != NULL && i < 6){
        fields[i++] = token;
        token = strtok(NULL, " ");
    }

    // Catch invalid FEN notation
    if(i < 6){
        fprintf(stderr, "%s", "Error: Invalid FEN (missing fields)\n");
        free(fen_copy);
        exit(EXIT_FAILURE);
    }

    // Reset bitboards and game states
    ResetBoardState();

    // Initialize Board Layout
    int rank = 7;
    int file = 0;

    for(int i = 0; i < strlen(fields[0]); i++){
        // Check if you reach a delimiter
        if(FEN[i] == '/'){
            rank--;
            file = 0;
            continue;
        }
        
        // If you get an int n skip n squares 
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

    // Current side
    side = (fields[1][0] == 'w') ? 0 : 1;

    // Castling rights
    for (int j = 0; j < strlen(fields[2]); j++) {
        // bitmask to determine current castling rights
        switch(fields[2][j]) {
            case 'K': castle |= 1 << 0; break; // White kingside
            case 'Q': castle |= 1 << 1; break; // White queenside
            case 'k': castle |= 1 << 2; break; // Black kingside
            case 'q': castle |= 1 << 3; break; // Black queenside
        }
    }

    // En passant 
    if (fields[3][0] == '-') enpassant = -1;
    else {
        int file_ep = fields[3][0] - 'a';
        int rank_ep = fields[3][1] - '1';
        enpassant = rank_ep * 8 + file_ep;
    }

    // Halfmove clock
    halfmove = atoi(fields[4]);

    // Fullmove number 
    fullmove = atoi(fields[5]);

    // release fen copy
    free(fen_copy);
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