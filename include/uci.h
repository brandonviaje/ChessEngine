#ifndef UCI_H
#define UCI_H

#include "MoveGen.h"
#include "Board.h"
#include "Search.h"

#define INPUT_BUFFER 4000

void ParseGo(char *line);
void ParsePosition(char *lineIn);
int ParseMove(char *char_ptr, int side);
void UciLoop();

#endif
