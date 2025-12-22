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
        int from = __builtin_ctzll(pawns); // index of LSB pawn
        U64 fromMask = 1ULL << from;

        // Capture moves
        U64 attacks = pawnAttacks[side][from] & enemyPieces;
        while (attacks)
        {
            int to = __builtin_ctzll(attacks);
            int captured = DetectCapture(to);

            // Promotion captures
            if ((side == WHITE && (fromMask & RANK_7)) || (side == BLACK && (fromMask & RANK_2)))
            {
                list->moves[list->count].prevEnpassant = enpassant;
                AddPromotionMoves(from, to, captured, side, list);
            }
            else
            {
                list->moves[list->count].prevEnpassant = enpassant;
                AddMove(list, (Move){piece, from, to, -1, captured, FLAG_NONE, enpassant});
            }

            attacks &= attacks - 1; // pop LSB
        }

        // Single push
        int to = (side == WHITE) ? from + 8 : from - 8;
        if (!GetBit(occupied, to))
        {
            // Promotion push
            if ((side == WHITE && (fromMask & RANK_7)) || (side == BLACK && (fromMask & RANK_2)))
            {
                list->moves[list->count].prevEnpassant = enpassant;
                AddPromotionMoves(from, to, -1, side, list);
            }
            else
            {
                list->moves[list->count].prevEnpassant = enpassant;
                AddMove(list, (Move){piece, from, to, -1, -1, FLAG_NONE, enpassant});

                // Double push
                if ((side == WHITE && (fromMask & RANK_2)) || (side == BLACK && (fromMask & RANK_7)))
                {
                    int over = (side == WHITE) ? from + 8 : from - 8;
                    int to2 = (side == WHITE) ? from + 16 : from - 16;
                    if (!GetBit(occupied, over) && !GetBit(occupied, to2))
                    {
                        list->moves[list->count].prevEnpassant = enpassant;
                        AddMove(list, (Move){piece, from, to2, -1, -1, FLAG_DOUBLE_PUSH, enpassant});
                    }
                }
            }
        }

        // En passant
        if (enpassant != -1)
        {
            U64 epMask = 1ULL << enpassant;
            if (pawnAttacks[side][from] & epMask)
            {
                int capPawn = (side == WHITE) ? p : P;
                list->moves[list->count].prevEnpassant = enpassant;
                AddMove(list, (Move){piece, from, enpassant, -1, capPawn, FLAG_ENPASSANT, enpassant});
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

    int from = __builtin_ctzll(king);
    int enemy = (piece == K) ? BLACK : WHITE;

    U64 enemyAttacks = GenerateAllAttacks(enemyPieces, occupied, enemy); // get current enemy attacks
    U64 moves = kingAttacks[from] & ~ownPieces & ~enemyAttacks;          // mask

    // Normal king moves
    while (moves)
    {
        int to = __builtin_ctzll(moves);
        int captured = DetectCapture(to);
        AddMove(list, (Move){piece, from, to, -1, captured, FLAG_NONE});
        moves &= moves - 1;
    }

    // Castling
    if (piece == K) // White king
    {
        // Kingside
        if (castle & 1) // K
        {
            if (!GetBit(occupied, 5) && !GetBit(occupied, 6) &&
                !IsSquareAttacked(4, BLACK) && !IsSquareAttacked(5, BLACK) && !IsSquareAttacked(6, BLACK))
            {
                AddMove(list, (Move){K, 4, 6, -1, -1, FLAG_CASTLE_KINGSIDE});
            }
        }

        // Queenside
        if (castle & (1 << 1)) // Q
        {
            if (!GetBit(occupied, 1) && !GetBit(occupied, 2) && !GetBit(occupied, 3) &&
                !IsSquareAttacked(4, BLACK) && !IsSquareAttacked(3, BLACK) && !IsSquareAttacked(2, BLACK))
            {
                AddMove(list, (Move){K, 4, 2, -1, -1, FLAG_CASTLE_QUEENSIDE});
            }
        }
    }
    else if (piece == k) // Black king
    {
        // Kingside
        if (castle & (1 << 2)) // k
        {
            if (!GetBit(occupied, 61) && !GetBit(occupied, 62) &&
                !IsSquareAttacked(60, WHITE) && !IsSquareAttacked(61, WHITE) && !IsSquareAttacked(62, WHITE))
            {
                AddMove(list, (Move){k, 60, 62, -1, -1, FLAG_CASTLE_KINGSIDE});
            }
        }

        // Queenside
        if (castle & (1 << 3)) // q
        {
            if (!GetBit(occupied, 57) && !GetBit(occupied, 58) && !GetBit(occupied, 59) &&
                !IsSquareAttacked(60, WHITE) && !IsSquareAttacked(59, WHITE) && !IsSquareAttacked(58, WHITE))
            {
                AddMove(list, (Move){k, 60, 58, -1, -1, FLAG_CASTLE_QUEENSIDE});
            }
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

int IsSquareAttacked(int sq, int bySide)
{
    if (pawnAttacks[bySide][sq] & bitboards[bySide == WHITE ? P : p])
        return 1;

    if (knightAttacks[sq] & bitboards[bySide == WHITE ? N : n])
        return 1;

    if (GetRookAttacks(sq, occupied) & (bitboards[bySide == WHITE ? R : r] | bitboards[bySide == WHITE ? Q : q]))
        return 1;

    if (GetBishopAttacks(sq, occupied) & (bitboards[bySide == WHITE ? B : b] | bitboards[bySide == WHITE ? Q : q]))
        return 1;

    return 0;
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

int IsKingInCheck(int checkSide)
{
    int enemySide = checkSide ^ 1;
    int kingSq = __builtin_ctzll(bitboards[checkSide == WHITE ? K : k]);

    if (pawnAttacks[enemySide][kingSq] & bitboards[enemySide == WHITE ? P : p])
        return 1;

    if (knightAttacks[kingSq] & bitboards[enemySide == WHITE ? N : n])
        return 1;

    if (GetRookAttacks(kingSq, occupied) & (bitboards[enemySide == WHITE ? R : r] | bitboards[enemySide == WHITE ? Q : q]))
        return 1;

    if (GetBishopAttacks(kingSq, occupied) & (bitboards[enemySide == WHITE ? B : b] | bitboards[enemySide == WHITE ? Q : q]))
        return 1;

    return 0;
}

void GenerateLegalMoves(MoveList *legal)
{
    MoveList pseudo;
    GenerateMoves(&pseudo);

    ResetMoveList(legal);

    for (int i = 0; i < pseudo.count; i++)
    {
        // make ith pseudo-legal move
        MakeMove(&pseudo, i);

        // check the side that just moved
        int movedSide = side ^ 1;

        // check if it is a legal move
        if (!IsKingInCheck(movedSide))
        {
            AddMove(legal, pseudo.moves[i]);
        }

        // undo same move
        UndoMove(&pseudo, i);
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
