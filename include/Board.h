#ifndef BITBOARD_H
#define BITBOARD_H

#include "MyTypes.h"
#include "Zobrist.h"

// Function Prototypes
void UpdateBitboards();
void ResetBoardState();
int CharToPiece(char c);
void SetPiece(int piece, int square);
void PrintBitBoard(U64 board);
void ParseFEN(char *FEN);
void MakeMove(Move *move);
void UndoMove(Move *move);
void MovePiece(int piece, U64 fromMask, U64 toMask);
void RestorePiece(int piece, U64 mask);
void UpdateCastlingRights(int piece, int from, int to, int captured);

#endif
