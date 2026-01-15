#ifndef UCI_H
#define UCI_H

#include "Search.h"
#include "MoveGen.h"
#define INPUT_BUFFER 4000

void ParseGo(char *line);
void ParsePosition(char *lineIn);
void UciLoop();

#endif
