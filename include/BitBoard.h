#ifndef BITBOARD_H
#define BITBOARD_H
#include "MyTypes.h"
#include "MoveGen.h"
#include "Attack.h"

// Piece bitboards
extern U64 bitboards[12];
extern U64 whitePieces;
extern U64 blackPieces;
extern U64 occupied;

// Game state
extern int side;                // 0 = white, 1 = black
extern int enpassant;           // en passant square
extern unsigned char castle;    // castling rights
extern int halfmove;            // halfmove clock
extern int fullmove;            // fullmove number

// Function Prototypes
void ResetBoardState();
int CharToPiece(char c);
void SetPiece(int piece, int square);
void PrintBitboard(U64 board);
void PrintGameState();
void ParseFEN(char * FEN);
void MakeMove(int index);
void UndoMove(int index);
void MovePiece(int piece, U64 fromMask, U64 toMask);
void RestorePiece(int piece, U64 mask);

#endif 