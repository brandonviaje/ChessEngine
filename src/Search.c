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

    // base case: if depth is 0, return eval
    if (depth == 0)
    {
        info.nodes++;
        return Evaluate();
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