#include "../include/Perft.h"

U64 Perft(int depth)
{
    MoveList list;
    U64 nodes = 0;

    if(depth == 0) return 1;

    GenerateMoves(&list); 

    for(int i=0; i<list.count; i++){

        MakeMove(&list, i);

        if(!IsInCheck()) // filter out illegal moves
            nodes += Perft(depth-1);

        UndoMove(&list, i);
    }

    return nodes;
}
