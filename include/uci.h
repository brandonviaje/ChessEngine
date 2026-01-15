#ifndef UCI_H
#define UCI_H

#include "MoveGen.h"
#include "Search.h"

#define INPUT_BUFFER 4000

void ParseGo(char *line);
void ParsePosition(char *lineIn);
void UciLoop();

#endif
