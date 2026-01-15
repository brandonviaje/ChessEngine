#include "../include/Search.h"

S_SEARCHINFO info;

void ClearSearch()
{
    info.nodes = 0;
    info.quit = 0;
}

// alpha beta search
int AlphaBeta(int alpha, int beta, int depth)
{

    // base case: if depth is 0, enter quiescence search
    if (depth == 0)
    {
        return Quiescence(alpha, beta);
    }

    info.nodes++; // count this node

    // generate pesudo legal moves
    MoveList list;
    GenerateMoves(&list);

    int legalMoves = 0;
    int score = -INF;

    // loop through all moves
    for (int i = 0; i < list.count; i++)
    {

        // make move
        MakeMove(&list, i);

        // check if move was legal
        int movedSide = side ^ 1;

        if (IsKingInCheck(movedSide))
        {
            UndoMove(&list, i); // illegal move, undo
            continue;           // Skip to next move
        }

        legalMoves++;

        // flip board and search deeper
        score = -AlphaBeta(-beta, -alpha, depth - 1);

        // take move back
        UndoMove(&list, i);

        // pruning
        if (score >= beta)
        {
            return beta; // beta Cutoff
        }

        if (score > alpha)
        {
            alpha = score; // found a better move
        }
    }

    // look for checkmate / stalemate
    if (legalMoves == 0)
    {
        // if no moves, check if we are in check
        if (IsKingInCheck(side))
        {
            return -MATE + (10 - depth); // checkmate
        }
        else
        {
            return 0; // stalemate
        }
    }

    return alpha;
}

// root search
void SearchPosition(int depth)
{
    ClearSearch();

    // best move tracking
    Move bestMove = {0};
    int bestScore = -INF;
    int currentScore = 0;
    int alpha = -INF;
    int beta = INF;

    MoveList list;
    GenerateMoves(&list);

    printf("Thinking to depth %d...\n", depth);

    for (int i = 0; i < list.count; i++)
    {
        // make move
        MakeMove(&list, i);

        // check if legal move
        int movedSide = side ^ 1;
        if (IsKingInCheck(movedSide))
        {
            UndoMove(&list, i);
            continue;
        }

        // search
        currentScore = -AlphaBeta(-beta, -alpha, depth - 1);

        // backtrack and explore more
        UndoMove(&list, i);

        if (currentScore > bestScore)
        {
            bestScore = currentScore;
            bestMove = list.moves[i];
            alpha = currentScore;
        }
    }

    printf("Search Depth: %d\n", depth);
    printf("Nodes visited: %ld\n", info.nodes);
    printf("Best Score: %d (centipawns)\n", bestScore);

    // print best move
    char f1 = 'a' + (bestMove.from % 8);
    char r1 = '1' + (bestMove.from / 8);
    char f2 = 'a' + (bestMove.to % 8);
    char r2 = '1' + (bestMove.to / 8);

    printf("Best Move: %c%c%c%c\n", f1, r1, f2, r2);
}

int Quiescence(int alpha, int beta)
{
    info.nodes++;

    int score = Evaluate();

    // if score way better than what opponent would accept, return
    if (score >= beta)
    {
        return beta;
    }

    // if position is better than anything we've seen, keep
    if (score > alpha)
    {
        alpha = score;
    }

    MoveList list;
    GenerateMoves(&list);

    for (int i = 0; i < list.count; i++)
    {

        Move m = list.moves[i];

        // only look at captures!
        if (m.captured == -1 && !(m.flags & FLAG_PROMOTION))
        {
            continue;
        }

        MakeMove(&list, i);

        // check legality
        int movedSide = side ^ 1;
        if (IsKingInCheck(movedSide))
        {
            UndoMove(&list, i);
            continue;
        }

        // call quiscence again
        int tempScore = -Quiescence(-beta, -alpha);

        UndoMove(&list, i);

        // beta cutoff
        if (tempScore >= beta)
        {
            return beta;
        }

        // alpha update
        if (tempScore > alpha)
        {
            alpha = tempScore;
        }
    }

    return alpha;
}