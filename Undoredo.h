#ifndef UNDOREDO_H
#define UNDOREDO_H

#include "Map.h"

struct URState
{
  RECT CellsPos;
  LevelCell* Cells;
  BOOL Link;
};

BOOL Undo(Duke2Map* Map, BOOL CheckAvailabilityOnly = FALSE);
BOOL Redo(Duke2Map* Map);
void AddUndoState(RECT* CellsPos, LevelCell* Cells, BOOL Link = FALSE);
void CleanUpUndoRedo(void);
BOOL NewUndoChain(void);

#endif