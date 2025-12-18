#include "../include/MoveGen.h"

// BitBoards
extern U64 bitboards[12];
extern U64 blackPieces;
extern U64 whitePieces;
extern U64 occupied;

// Game State
extern int side;
extern int enpassant;
extern unsigned char castle;

// Attack Tables
extern U64 knightAttacks[64];
extern U64 kingAttacks[64];
extern U64 pawnAttacks[2][64]; // 0 white, 1 black

void GeneratePawnMoves(U64 pawns, U64 ownPieces, U64 enemyPieces, int side, int piece, MoveList *list)
{
    if (!pawns)
        return;

    while (pawns)
    {
        int from = __builtin_ctzll(pawns);

        // pawn captures
        U64 attacks = pawnAttacks[side][from] & enemyPieces;

        while (attacks)
        {
            int to = __builtin_ctzll(attacks);
            int captured = DetectCapture(to);

            // promotion capture
            if ((side == WHITE && ((1ULL << from) & RANK_7)) || (side == BLACK && ((1ULL << from) & RANK_2)))
            {
                AddPromotionMoves(from, to, captured, side, list);
            }
            else
            {
                AddMove(list, (Move){piece, from, to, -1, captured, FLAG_NONE});
            }

            attacks &= attacks - 1;
        }

        // single push

        int to = (side == WHITE) ? from + 8 : from - 8;

        if (!(occupied & (1ULL << to)))
        {
            // promotion push
            if ((side == WHITE && ((1ULL << from) & RANK_7)) ||
                (side == BLACK && ((1ULL << from) & RANK_2)))
            {
                AddPromotionMoves(from, to, -1, side, list);
            }
            else
            {
                AddMove(list, (Move){piece, from, to, -1, -1, FLAG_NONE});

                // double push
                if (side == WHITE && ((1ULL << from) & RANK_2))
                {
                    int to2 = from + 16;
                    if (!(occupied & (1ULL << to2)))
                        AddMove(list, (Move){piece, from, to2, -1, -1, FLAG_DOUBLE_PUSH});
                }
                else if (side == BLACK && ((1ULL << from) & RANK_7))
                {
                    int to2 = from - 16;
                    if (!(occupied & (1ULL << to2)))
                        AddMove(list, (Move){piece, from, to2, -1, -1, FLAG_DOUBLE_PUSH});
                }
            }
        }

        // handle enpassant
        if (enpassant != -1)
        {
            U64 epMask = 1ULL << enpassant;

            if (pawnAttacks[side][from] & epMask)
            {
                int capturedSquare = (side == WHITE) ? enpassant - 8 : enpassant + 8;
                AddMove(list, (Move){piece, from, enpassant, -1, DetectCapture(capturedSquare), FLAG_ENPASSANT});
            }
        }

        pawns &= pawns - 1; // pop LSB
    }
}

void GenerateKnightMoves(U64 knights, U64 ownPieces, U64 enemyPieces, int piece, MoveList *list)
{
    if (!knights)
        return;

    // process knights
    while (knights)
    {
        int from = __builtin_ctzll(knights);            // process current knight position
        U64 attacks = knightAttacks[from] & ~ownPieces; // get precomputed attack table

        // add attack to move list
        while (attacks)
        {
            int to = __builtin_ctzll(attacks); // get current attack square
            int captured = DetectCapture(to);
            AddMove(list, (Move){piece, from, to, -1, captured, FLAG_NONE}); // add to move list
            attacks &= (attacks - 1);                                        // pop LSB
        }
        knights &= (knights - 1); // pop LSB
    }
}

void GenerateKingMoves(U64 king, U64 ownPieces, U64 enemyPieces, int piece, MoveList *list)
{
    if (!king)
        return;

    int from = __builtin_ctzll(king);           // get current king position
    U64 moves = kingAttacks[from] & ~ownPieces; // get potential moves

    // process normal moves
    while (moves)
    {
        int to = __builtin_ctzll(moves);                                 // get current possible move position
        int captured = DetectCapture(to);                                // detect if there is piece on it
        AddMove(list, (Move){piece, from, to, -1, captured, FLAG_NONE}); // add to move list
        moves &= (moves - 1);                                            // pop LSB
    }

    // handle castling
    if (piece == K)
    {
        // kingside white
        if ((castle & (1 << 0)) && !(occupied & ((1ULL << 5) | (1ULL << 6))))
        {
            AddMove(list, (Move){piece, from, 6, -1, -1, FLAG_CASTLE_KINGSIDE});
        }

        // queenside white
        if ((castle & (1 << 1)) && !(occupied & ((1ULL << 1) | (1ULL << 2) | (1ULL << 3))))
        {
            AddMove(list, (Move){piece, from, 2, -1, -1, FLAG_CASTLE_QUEENSIDE});
        }
    }
    else if (piece == k)
    {
        // kingside black
        if ((castle & (1 << 2)) && !(occupied & ((1ULL << 61) | (1ULL << 62))))
        {
            AddMove(list, (Move){piece, from, 62, -1, -1, FLAG_CASTLE_KINGSIDE});
        }

        // queenside black
        if ((castle & (1 << 3)) && !(occupied & ((1ULL << 57) | (1ULL << 58) | (1ULL << 59))))
        {
            AddMove(list, (Move){piece, from, 58, -1, -1, FLAG_CASTLE_QUEENSIDE});
        }
    }
}

void GenerateRookMoves(U64 rooks, U64 ownPieces, U64 enemyPieces, int piece, MoveList *list)
{
    if (!rooks)
        return;

    // process rooks
    while (rooks)
    {
        int from = __builtin_ctzll(rooks);                       // current position of rook
        U64 moves = GetRookAttacks(from, occupied) & ~ownPieces; // magic attack table, remove friendly pieces

        while (moves)
        {
            int to = __builtin_ctzll(moves);                                 // get current possible move
            int captured = DetectCapture(to);                                // detect captures
            AddMove(list, (Move){piece, from, to, -1, captured, FLAG_NONE}); // add to move list
            moves &= (moves - 1);                                            // pop LSB
        }

        rooks &= (rooks - 1); // pop LSB
    }
}

void GenerateBishopMoves(U64 bishops, U64 ownPieces, U64 enemyPieces, int piece, MoveList *list)
{
    if (!bishops)
        return;

    // process bishops
    while (bishops)
    {
        int from = __builtin_ctzll(bishops);                       // current position of bishop
        U64 moves = GetBishopAttacks(from, occupied) & ~ownPieces; // magic attack table, remove friendly pieces

        while (moves)
        {
            int to = __builtin_ctzll(moves);                                 // get current possible move
            int captured = DetectCapture(to);                                // detect captures
            AddMove(list, (Move){piece, from, to, -1, captured, FLAG_NONE}); // add to move list
            moves &= (moves - 1);                                            // pop LSB
        }

        bishops &= (bishops - 1); // pop LSB
    }
}

void GenerateQueenMoves(U64 queen, U64 ownPieces, U64 enemyPieces, int piece, MoveList *list)
{
    if (!queen)
        return;

    // queen moves is the union of rook and bishop generated moves
    GenerateRookMoves(queen, ownPieces, enemyPieces, piece, list);
    GenerateBishopMoves(queen, ownPieces, enemyPieces, piece, list);
}

// Detect Captures

int DetectCapture(int to)
{
    // white side: start at black piece bb black side: start at white piece bb
    int start = (side == WHITE) ? 6 : 0;
    int end = (side == WHITE) ? 12 : 6;

    U64 target = 0ULL;
    SetBit(target, to);

    for (int i = start; i < end; i++)
    {
        if (bitboards[i] & target)
            return i; // captured piece index
    }

    return -1; // no capture
}

// Add promotion moves for pawns
void AddPromotionMoves(int from, int to, int captured, int side, MoveList *list)
{
    if (side == WHITE)
    {
        AddMove(list, (Move){P, from, to, Q, captured, FLAG_PROMOTION});
        AddMove(list, (Move){P, from, to, R, captured, FLAG_PROMOTION});
        AddMove(list, (Move){P, from, to, B, captured, FLAG_PROMOTION});
        AddMove(list, (Move){P, from, to, N, captured, FLAG_PROMOTION});
    }
    else
    {
        AddMove(list, (Move){p, from, to, q, captured, FLAG_PROMOTION});
        AddMove(list, (Move){p, from, to, r, captured, FLAG_PROMOTION});
        AddMove(list, (Move){p, from, to, b, captured, FLAG_PROMOTION});
        AddMove(list, (Move){p, from, to, n, captured, FLAG_PROMOTION});
    }
}

// Add generic Moves to move list
void AddMove(MoveList *list, Move m)
{
    if (list->count < MAX_MOVES)
        list->moves[list->count++] = m;
    else
    {
        fprintf(stderr, "MoveList overflow!\n");
        exit(1);
    }
}

// Check if king is in check by enemy pieces
int IsInCheck()
{
    int enemySide = side ^ 1;
    int kingSq = __builtin_ctzll(bitboards[side == WHITE ? K : k]);

    // Check pawn attacks
    if (pawnAttacks[enemySide][kingSq] & bitboards[enemySide == WHITE ? P : p])
        return 1;

    // Check knight attacks
    if (knightAttacks[kingSq] & bitboards[enemySide == WHITE ? N : n])
        return 1;

    // Check king attacks
    if (kingAttacks[kingSq] & bitboards[enemySide == WHITE ? K : k])
        return 1;

    // Check rook / queen attacks along orthogonals
    U64 rookBB = bitboards[enemySide == WHITE ? R : r] | bitboards[enemySide == WHITE ? Q : q];
    if (GetRookAttacks(kingSq, occupied) & rookBB)
        return 1;

    // Check bishop / queen attacks along diagonals
    U64 bishopBB = bitboards[enemySide == WHITE ? B : b] | bitboards[enemySide == WHITE ? Q : q];
    if (GetBishopAttacks(kingSq, occupied) & bishopBB)
        return 1;

    return 0; // king is safe
}

// Reset Move List and Move Count
void ResetMoveList(MoveList *list)
{
    memset(list->moves, 0, sizeof(list->moves)); // clear out moves
    list->count = 0;                             // reset count
}

void GenerateMovesInternal(U64 Pawn, U64 Knight, U64 Bishop, U64 Rook, U64 Queen, U64 King, U64 ownPieces, U64 enemyPieces, int side, MoveList *list)
{
    ResetMoveList(list);
    GeneratePawnMoves(Pawn, ownPieces, enemyPieces, side, side == WHITE ? P : p, list);
    GenerateKnightMoves(Knight, ownPieces, enemyPieces, side == WHITE ? N : n, list);
    GenerateBishopMoves(Bishop, ownPieces, enemyPieces, side == WHITE ? B : b, list);
    GenerateRookMoves(Rook, ownPieces, enemyPieces, side == WHITE ? R : r, list);
    GenerateQueenMoves(Queen, ownPieces, enemyPieces, side == WHITE ? Q : q, list);
    GenerateKingMoves(King, ownPieces, enemyPieces, side == WHITE ? K : k, list);
}

void GenerateMoves(MoveList *list)
{
    if (side == WHITE)
    {
        GenerateMovesInternal(bitboards[P], bitboards[N], bitboards[B], bitboards[R], bitboards[Q], bitboards[K], whitePieces, blackPieces, side, list); // white
    }
    else
    {
        GenerateMovesInternal(bitboards[p], bitboards[n], bitboards[b], bitboards[r], bitboards[q], bitboards[k], blackPieces, whitePieces, side, list); // black
    }
}

void PrintMoveList(MoveList *list)
{
    printf("Move List Count: %d\n\n", list->count);
    for (int i = 0; i < list->count; i++)
    {
        int from = list->moves[i].from;
        int to = list->moves[i].to;
        char file_from = 'a' + (from % 8);
        char rank_from = '1' + (from / 8);
        char file_to = 'a' + (to % 8);
        char rank_to = '1' + (to / 8);
        printf("Move %d: %c%c -> %c%c\n", i + 1, file_from, rank_from, file_to, rank_to);
    }
}