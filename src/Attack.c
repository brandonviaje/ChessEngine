#include "Attack.h"

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


int IsInCheck(){

    // Generate pseudo legal moves after making a move
    GeneratePseudoLegalMoves(side == WHITE ? whitePieces : blackPieces, side == WHITE ? blackPieces : whitePieces, side);
    U64 kingPosition = side == WHITE ? bitboards[K] : bitboards[k];

    // iterate through move list, if the piece type is of the opposite color, check if the to part is where the king resides
    for(int i = 0; i <= moveCount; i++){
        if(moveList[i].piece <= K && side == BLACK){
            if(moveList[i].to == kingPosition){
                return 1;
            }
        }else{
            if(moveList[i].to == kingPosition){
                return 1;
            }
        }   
    }
    return 0; // did not find any moves 
}