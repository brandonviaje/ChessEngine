#include "../include/Perft.h"

U64 Perft(int depth)
{
    // base case
    if (depth == 0)
        return 1;

    MoveList list;
    GenerateLegalMoves(&list);  

    U64 nodes = 0;

    for (int i = 0; i < list.count; i++)
    {
        MakeMove(&list, i);
        nodes += Perft(depth - 1);
        UndoMove(&list, i);
    }

    return nodes;
}

void PerftDivide(int depth)
{
    MoveList list;
    GenerateLegalMoves(&list);

    U64 total = 0;

    for (int i = 0; i < list.count; i++)
    {
        MakeMove(&list, i);

        U64 nodes = Perft(depth - 1);

        char fromFile = 'a' + (list.moves[i].from % 8);
        char fromRank = '1' + (list.moves[i].from / 8);
        char toFile   = 'a' + (list.moves[i].to % 8);
        char toRank   = '1' + (list.moves[i].to / 8);

        printf("%c%c%c%c: %llu\n", fromFile, fromRank, toFile, toRank, nodes);
        total += nodes;

        UndoMove(&list, i);
    }

    printf("Total nodes: %llu\n", total);
}
