#include "../include/Search.h"

S_SEARCHINFO info;

// killer moves table, used for move ordering to try and trigger beta cutoffs faster
Move killerMoves[MAX_PLY][2];

void ClearSearch()
{
    // reset search info and clear killer moves before we start
    info.nodes = 0;
    info.quit = 0;
    memset(killerMoves, 0, sizeof(killerMoves));
}

int AlphaBeta(int alpha, int beta, int depth)
{
    // if we hit depth 0, drop into quiescence search instead
    if (depth == 0)
        return Quiescence(alpha, beta);

    info.nodes++; // count nodes for stats

    MoveList list;
    GenerateMoves(&list); // generate all moves for this position

    int legalMoves = 0;
    int score = -INF;

    // fetch killer moves for this depth to help ordering
    Move k1 = {0}, k2 = {0};
    if (depth < MAX_PLY)
    {
        k1 = killerMoves[depth][0];
        k2 = killerMoves[depth][1];
    }

    for (int i = 0; i < list.count; i++)
    {
        // move picker uses killers to try better moves first
        PickNextMove(&list, i, k1, k2);

        Move m = list.moves[i];
        MakeMove(&list, i);

        int movedSide = side ^ 1;
        if (IsKingInCheck(movedSide))
        {
            UndoMove(&list, i);
            continue; // skip illegal moves
        }

        legalMoves++;
        score = -AlphaBeta(-beta, -alpha, depth - 1);
        UndoMove(&list, i);

        // beta cutoff
        if (score >= beta)
        {
            // store killer moves if it's not a capture
            if (m.captured == -1 && depth < MAX_PLY)
            {
                killerMoves[depth][1] = killerMoves[depth][0];
                killerMoves[depth][0] = m;
            }
            return beta;
        }

        if (score > alpha)
            alpha = score; // best so far
    }

    if (legalMoves == 0)
        return IsKingInCheck(side) ? -MATE + (10 - depth) : 0;

    return alpha;
}

void SearchPosition(int depth)
{
    ClearSearch();

    Move bestMove = {0};
    int bestScore = -INF;
    int currentScore = 0;
    int alpha = -INF;
    int beta = INF;

    MoveList list;
    GenerateMoves(&list);

    printf("Thinking to depth %d...\n", depth);

    Move k1 = {0}, k2 = {0}; // dummy killers for root

    for (int i = 0; i < list.count; i++)
    {
        PickNextMove(&list, i, k1, k2);
        MakeMove(&list, i);

        int movedSide = side ^ 1;
        if (IsKingInCheck(movedSide))
        {
            UndoMove(&list, i);
            continue;
        }

        currentScore = -AlphaBeta(-beta, -alpha, depth - 1);
        UndoMove(&list, i);

        if (currentScore > bestScore)
        {
            bestScore = currentScore;
            bestMove = list.moves[i];
            alpha = currentScore;
        }
    }

    printf("search depth: %d\n", depth);
    printf("nodes visited: %ld\n", info.nodes);
    printf("best score: %d (centipawns)\n", bestScore);

    char f1 = 'a' + (bestMove.from % 8);
    char r1 = '1' + (bestMove.from / 8);
    char f2 = 'a' + (bestMove.to % 8);
    char r2 = '1' + (bestMove.to / 8);

    printf("best move: %c%c%c%c\n", f1, r1, f2, r2);
}

int Quiescence(int alpha, int beta)
{
    info.nodes++;

    int score = Evaluate(); // static eval first

    if (score >= beta)
        return beta;
    if (score > alpha)
        alpha = score;

    MoveList list;
    GenerateMoves(&list);

    Move k1 = {0}, k2 = {0}; // no killers in q-search

    for (int i = 0; i < list.count; i++)
    {
        PickNextMove(&list, i, k1, k2);
        Move m = list.moves[i];

        if (m.captured == -1 && !(m.flags & FLAG_PROMOTION))
            continue; // skip quiet moves

        MakeMove(&list, i);

        int movedSide = side ^ 1;
        if (IsKingInCheck(movedSide))
        {
            UndoMove(&list, i);
            continue;
        }

        int tempScore = -Quiescence(-beta, -alpha);
        UndoMove(&list, i);

        if (tempScore >= beta)
            return beta;
        if (tempScore > alpha)
            alpha = tempScore;
    }

    return alpha;
}