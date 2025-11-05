#ifndef BITBOARD_H
#define BITBOARD_H
#include "Masks.h"
#include "MoveGen.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#define starting_position "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define tricky_position "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b KQkq - 0 1"

// Piece bitboards
extern U64 bitboards[12];
extern U64 whitePieces;
extern U64 blackPieces;
extern U64 occupied;

// Game state
extern int side;          // 0 = white, 1 = black
extern int enpassant;     // en passant square
extern unsigned char castle; // castling rights
extern int halfmove;      // halfmove clock
extern int fullmove;      // fullmove number

// Function Prototypes
void ResetBoardState();
int CharToPiece(char c);
void SetPiece(int piece, int square);
void RemovePiece(int piece, int square);
void PrintBitboard(U64 board);
void PrintGameState();
void ParseFEN(char * FEN);

#endif