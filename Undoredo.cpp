/** Copyright 2023 Nikolai Wuttke-Hohendorf
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the “Software”),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */


#pragma warning(disable : 4786)

#include "Undoredo.h"
#include <list>

int UndoLevels = 20;

static list<URState> States;
static list<URState>::iterator StatesIt, LastState;
static BOOL NewUndoChainOpened;

BOOL NewUndoChain(void)
{
  return NewUndoChainOpened;
}

void TransferCurrentStateToMap(Duke2Map* Map)
{
  POINT Pos;
  LevelCell Cell;

  for (int y = StatesIt->CellsPos.top; y < StatesIt->CellsPos.bottom; y++)
  {
    for (int x = StatesIt->CellsPos.left; x < StatesIt->CellsPos.right; x++)
    {
      Pos.x = x;
      Pos.y = y;

      memcpy(
        &Cell,
        &(StatesIt->Cells
            [x - StatesIt->CellsPos.left +
             (y - StatesIt->CellsPos.top) *
               (StatesIt->CellsPos.right - StatesIt->CellsPos.left)]),
        sizeof(LevelCell));
      Map->SetCellAttributes(&Pos, Cell.Solid, Cell.Masked, Cell.bMasked);

      if (Cell.Actor)
        Map->SetActor(&Pos, Cell.ActorID);
      else
        Map->DeleteActor(&Pos);
    }
  }
}

BOOL Undo(Duke2Map* Map, BOOL CheckAvailabilityOnly)
{
  if (States.empty() || StatesIt == States.begin())
    return FALSE;

  BOOL Res = FALSE, Link = StatesIt->Link;

  if (CheckAvailabilityOnly)
    return StatesIt != States.begin();

  StatesIt--;

  TransferCurrentStateToMap(Map);

  if (StatesIt != States.begin())
  {
    StatesIt--;

    if (StatesIt != States.begin())
      Res = TRUE;
  }

  if (Link)
    return Undo(Map);

  return Res;
}

BOOL Redo(Duke2Map* Map)
{
  if (States.empty() || StatesIt == LastState)
    return FALSE;

  if (StatesIt != States.begin())
    StatesIt++;

  BOOL Res = FALSE, Link = StatesIt->Link;

  StatesIt++;

  TransferCurrentStateToMap(Map);

  if (StatesIt != LastState)
    Res = TRUE;

  if (Link)
    return Redo(Map);

  return Res;
}

void AddUndoState(RECT* CellsPos, LevelCell* Cells, BOOL Link)
{
  NewUndoChainOpened = FALSE;

  URState State;
  State.Link = Link;
  memcpy(&State.CellsPos, CellsPos, sizeof(RECT));
  State.Cells = new LevelCell
    [(State.CellsPos.right - State.CellsPos.left) *
     (State.CellsPos.bottom - State.CellsPos.top)];
  memcpy(
    State.Cells,
    Cells,
    sizeof(LevelCell) * (State.CellsPos.right - State.CellsPos.left) *
      (State.CellsPos.bottom - State.CellsPos.top));

  if (!States.empty() && StatesIt != LastState)
  {
    NewUndoChainOpened = TRUE;

    if (StatesIt != States.begin())
      StatesIt++;

    for (list<URState>::iterator it = StatesIt; it != States.end(); it++)
      delete it->Cells;

    States.erase(StatesIt, States.end());
  }

  States.push_back(State);

  StatesIt = States.end();
  StatesIt--;
  LastState = StatesIt;

  if (States.size() > UndoLevels)
  {
    delete States.front().Cells;
    States.pop_front();
  }
}

void CleanUpUndoRedo(void)
{
  while (States.size())
  {
    delete States.front().Cells;
    States.pop_front();
  }
}

void RemoveLastState(void)
{
  States.pop_front();
}