#include "MyTypes.h"
#include "BitBoard.h"
#include "MoveGen.h"

// Function Prototype
int IsInCheck();

typedef struct {
  int from;
  int to;
  int captured;
} AttackGeneration;