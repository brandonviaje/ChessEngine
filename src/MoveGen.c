#include "../include/MoveGen.h"

// Global variables
Move moveList[MAX_MOVES];
int moveCount = 0;

// BitBoards
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

void GeneratePawnMoves(U64 pawns, U64 ownPieces, U64 enemyPieces, int side, int piece)
{
    if (!pawns)
        return;
}

void GenerateKnightMoves(U64 knights, U64 ownPieces, U64 enemyPieces, int piece)
{
    if (!knights)
        return;
}

void GenerateKingMoves(U64 king, U64 ownPieces, U64 enemyPieces, int piece)
{
    if (!king)
        return;
}

void GenerateRookMoves(U64 rooks, U64 ownPieces, U64 enemyPieces, int piece)
{
    if (!rooks)
        return;
}

void GenerateBishopMoves(U64 bishops, U64 ownPieces, U64 enemyPieces, int piece)
{
    if (!bishops)
        return;
}

void GenerateQueenMoves(U64 queen, U64 ownPieces, U64 enemyPieces, int piece)
{
    if (!queen)
        return;
    // Queen moves is the union of rook and bishop generated moves
    GenerateRookMoves(queen, ownPieces, enemyPieces, piece);
    GenerateBishopMoves(queen, ownPieces, enemyPieces, piece);
}

void GenerateMovesInternal(U64 Pawn, U64 Knight, U64 Bishop, U64 Rook, U64 Queen, U64 King, U64 ownPieces, U64 enemyPieces, int side)
{
    ResetMoveList();
    GeneratePawnMoves(Pawn, ownPieces, enemyPieces, side, side == WHITE ? P : p);
    GenerateKnightMoves(Knight, ownPieces, enemyPieces, side == WHITE ? N : n);
    GenerateBishopMoves(Bishop, ownPieces, enemyPieces, side == WHITE ? B : b);
    GenerateRookMoves(Rook, ownPieces, enemyPieces, side == WHITE ? R : r);
    GenerateQueenMoves(Queen, ownPieces, enemyPieces, side == WHITE ? Q : q);
    GenerateKingMoves(King, ownPieces, enemyPieces, side == WHITE ? K : k);
}

void GenerateMoves()
{
    if (side == WHITE)
    { // white
        GenerateMovesInternal(bitboards[P], bitboards[N], bitboards[B], bitboards[R], bitboards[Q], bitboards[K], whitePieces, blackPieces, side);
    }
    else
    { // black
        GenerateMovesInternal(bitboards[p], bitboards[n], bitboards[b], bitboards[r], bitboards[q], bitboards[k], blackPieces, whitePieces, side);
    }
}

// Reset Move List and Move Count
void ResetMoveList()
{
    memset(moveList, 0, sizeof(moveList));
    moveCount = 0;
}

int DetectCapture(int to)
{
    // white side: start at black piece bb black side: start at white piece bb
    int start = (side == WHITE) ? 6 : 0;
    int end = (side == WHITE) ? 12 : 6;

    U64 target = 1ULL << to;

    for (int i = start; i < end; i++)
    {
        if (bitboards[i] & target)
            return i; // captured piece index
    }

    return -1; // no capture
}

// Add promotion moves for pawns
void AddPromotionMoves(int from, int to, int captured, int side)
{
    if (side == WHITE)
    {
        moveList[moveCount++] = (Move){P, from, to, Q, captured, FLAG_PROMOTION};
        moveList[moveCount++] = (Move){P, from, to, R, captured, FLAG_PROMOTION};
        moveList[moveCount++] = (Move){P, from, to, B, captured, FLAG_PROMOTION};
        moveList[moveCount++] = (Move){P, from, to, N, captured, FLAG_PROMOTION};
    }
    else
    {
        moveList[moveCount++] = (Move){p, from, to, q, captured, FLAG_PROMOTION};
        moveList[moveCount++] = (Move){p, from, to, r, captured, FLAG_PROMOTION};
        moveList[moveCount++] = (Move){p, from, to, b, captured, FLAG_PROMOTION};
        moveList[moveCount++] = (Move){p, from, to, n, captured, FLAG_PROMOTION};
    }
}

void PrintMoveList()
{
    printf("Move List Count: %d\n\n", moveCount);
    for (int i = 0; i < moveCount; i++)
    {
        int from = moveList[i].from;
        int to = moveList[i].to;
        char file_from = 'a' + (from % 8);
        char rank_from = '1' + (from / 8);
        char file_to = 'a' + (to % 8);
        char rank_to = '1' + (to / 8);
        printf("Move %d: %c%c -> %c%c\n", i + 1, file_from, rank_from, file_to, rank_to);
    }
}

int main()
{
    TestAllMagicAttacks();
    StressTestMagic();
    return 0;
}
