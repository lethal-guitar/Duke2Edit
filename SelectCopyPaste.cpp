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


#include "SelectCopyPaste.h"
#include "Undoredo.h"

SelectCopyPasteClass SelectCopyPaste;

void SelectCopyPasteClass::Deselect(POINT* ScrollPos, Duke2Map* Level)
{
  if (FloatingSelection)
  {
    PasteFromFloatingSelection(Level);
    delete FloatingSelection;
    FloatingSelection = NULL;
  }

  SelectionActive = FALSE;
}

BOOL SelectCopyPasteClass::FocusRectChange(POINT* Pos)
{
  if (!Dragging)
    return FALSE;

  Pos->x -= Pos->x % TILESIZE_EDITOR;
  Pos->y -= Pos->y % TILESIZE_EDITOR;

  DrawFocusRect(hDCFocus, &FocusRect);

  FocusRect.left = FocusOrigin.x;
  FocusRect.top = FocusOrigin.y;
  FocusRect.right = Pos->x + TILESIZE_EDITOR;
  FocusRect.bottom = Pos->y + TILESIZE_EDITOR;

  if (Pos->x < FocusRect.left)
  {
    FocusRect.left = FocusRect.right - TILESIZE_EDITOR;
    FocusRect.right = FocusOrigin.x;
  }

  if (Pos->y < FocusRect.top)
  {
    FocusRect.top = FocusRect.bottom - TILESIZE_EDITOR;
    FocusRect.bottom = FocusOrigin.y;
  }

  DrawFocusRect(hDCFocus, &FocusRect);

  return TRUE;
}

BOOL SelectCopyPasteClass::SelectionFinished(
  HWND hWndFD,
  POINT* ScrollPos,
  Duke2Map* Level,
  BOOL* Pasted)
{
  if (Pasted)
    *Pasted = FALSE;

  if (MovingSelection)
  {
    MovingSelection = FALSE;
    FloatingSelDst = FocusRect;
    OffsetRect(
      &FloatingSelDst,
      StartScrollPos.x * TILESIZE_EDITOR,
      StartScrollPos.y * TILESIZE_EDITOR);
    return FALSE;
  }

  if (Dragging)
  {
    EndFocusDragging(hWndFD);
    SelectionActive =
      FocusRect.right - FocusRect.left && FocusRect.bottom - FocusRect.top;

    if (FloatingSelection)
    {
      BOOL DidPaste = PasteFromFloatingSelection(Level);

      if (Pasted)
        *Pasted = DidPaste;
    }

    if (SelectionActive)
    {
      FloatingSelDst = FocusRect;
      SetRect(
        &FloatingSelOrigin,
        FocusRect.left / TILESIZE_EDITOR,
        FocusRect.top / TILESIZE_EDITOR,
        FocusRect.right / TILESIZE_EDITOR,
        FocusRect.bottom / TILESIZE_EDITOR);
      OffsetRect(&FloatingSelOrigin, ScrollPos->x, ScrollPos->y);
      CopyToFloatingSelection(ScrollPos, Level);
      Delete(ScrollPos, Level);
    }

    return TRUE;
  }

  return FALSE;
}

void SelectCopyPasteClass::BeginFocusDragging(
  POINT* Pos,
  POINT* ScrollPos,
  HWND hWndFD)
{
  if (PointInSelection(Pos, ScrollPos))
  {
    MovingSelection = TRUE;
    return;
  }

  Dragging = TRUE;
  SetRect(
    &FocusRect,
    Pos->x / TILESIZE_EDITOR * TILESIZE_EDITOR,
    Pos->y / TILESIZE_EDITOR * TILESIZE_EDITOR,
    0,
    0);
  FocusRect.right = FocusRect.left;
  FocusRect.bottom = FocusRect.top;
  hDCFocus = GetDC(hWndFD);
  DrawFocusRect(hDCFocus, &FocusRect);
  FocusOrigin.x = FocusRect.left;
  FocusOrigin.y = FocusRect.top;
}

void SelectCopyPasteClass::KillFloatingSelection(void)
{
  delete FloatingSelection;
  FloatingSelection = NULL;
  SelectionActive = FALSE;

  if (!FloatingSelIsFromClipboard)
  {
    AddUndoState(&FloatingSelOrigin, DelUndoBefore);
    AddUndoState(&FloatingSelOrigin, DelUndoAfter);
    delete DelUndoBefore;
    delete DelUndoAfter;
    DelUndoBefore = NULL;
    DelUndoAfter = NULL;
  }
}

void SelectCopyPasteClass::CopyToFloatingSelection(
  POINT* ScrollPos,
  Duke2Map* Level)
{
  RECT CellsPos = {
    FocusRect.left / TILESIZE_EDITOR,
    FocusRect.top / TILESIZE_EDITOR,
    FocusRect.right / TILESIZE_EDITOR,
    FocusRect.bottom / TILESIZE_EDITOR};
  OffsetRect(&CellsPos, ScrollPos->x, ScrollPos->y);
  OffsetRect(
    &FloatingSelDst,
    ScrollPos->x * TILESIZE_EDITOR,
    ScrollPos->y * TILESIZE_EDITOR);
  StartScrollPos = *ScrollPos;
  LevelCell Cell;
  delete FloatingSelection;

  FloatingSelection = new LevelCell
    [(CellsPos.right - CellsPos.left) * (CellsPos.bottom - CellsPos.top)];

  for (int y = CellsPos.top; y < CellsPos.bottom; y++)
  {
    for (int x = CellsPos.left; x < CellsPos.right; x++)
    {
      POINT Pos = {x, y};
      Level->GetCellAttributes(&Pos, &Cell);
      // Cell.Actor = FALSE;
      // Cell.ActorID = 0;
      FloatingSelection
        [x - CellsPos.left +
         (y - CellsPos.top) * (CellsPos.right - CellsPos.left)] = Cell;
    }
  }

  FloatingSelIsFromClipboard = FALSE;
}

BOOL SelectCopyPasteClass::PasteFromFloatingSelection(Duke2Map* Level)
{
  RECT CellsPos = {
    FloatingSelDst.left / TILESIZE_EDITOR,
    FloatingSelDst.top / TILESIZE_EDITOR,
    FloatingSelDst.right / TILESIZE_EDITOR,
    FloatingSelDst.bottom / TILESIZE_EDITOR};
  LevelCell Cell;

  POINT CellsPosSize = {
    CellsPos.right - CellsPos.left, CellsPos.bottom - CellsPos.top};

  LevelCell* UndoCellsBefore = new LevelCell[CellsPosSize.x * CellsPosSize.y];
  LevelCell* UndoCellsAfter = new LevelCell[CellsPosSize.x * CellsPosSize.y];

  for (int y = CellsPos.top; y < CellsPos.bottom; y++)
  {
    for (int x = CellsPos.left; x < CellsPos.right; x++)
    {
      POINT Pos = {x, y};
      Level->GetCellAttributes(&Pos, &Cell);
      UndoCellsBefore[x - CellsPos.left + (y - CellsPos.top) * CellsPosSize.x] =
        Cell;
      Cell = FloatingSelection
        [x - CellsPos.left + (y - CellsPos.top) * CellsPosSize.x];

      Level->SetCellAttributes(&Pos, Cell.Solid, Cell.Masked, Cell.bMasked);
      UndoCellsAfter[x - CellsPos.left + (y - CellsPos.top) * CellsPosSize.x] =
        Cell;

      if (Cell.Actor)
        Level->SetActor(&Pos, Cell.Actor);
      else
        Level->DeleteActor(&Pos);
    }
  }

  BOOL Res = FALSE;

  if (!FloatingSelIsFromClipboard)
  {
    if (!EqualRect(&CellsPos, &FloatingSelOrigin))
    {
      AddUndoState(&FloatingSelOrigin, DelUndoBefore, TRUE);
      AddUndoState(&FloatingSelOrigin, DelUndoAfter);
      AddUndoState(&CellsPos, UndoCellsBefore);
      AddUndoState(&CellsPos, UndoCellsAfter, TRUE);
      delete DelUndoBefore;
      delete DelUndoAfter;
      DelUndoBefore = NULL;
      DelUndoAfter = NULL;
      Res = TRUE;
    }
  }
  else
  {
    AddUndoState(&CellsPos, UndoCellsBefore);
    AddUndoState(&CellsPos, UndoCellsAfter);
    Res = TRUE;
  }

  delete UndoCellsBefore;
  delete UndoCellsAfter;
  delete FloatingSelection;
  FloatingSelection = NULL;

  return Res;
}

void SelectCopyPasteClass::Copy(HWND hWndCB, POINT* ScrollPos)
{
  OpenClipboard(hWndCB);
  EmptyClipboard();

  RECT CellsPos = {
    FocusRect.left / TILESIZE_EDITOR,
    FocusRect.top / TILESIZE_EDITOR,
    FocusRect.right / TILESIZE_EDITOR,
    FocusRect.bottom / TILESIZE_EDITOR};
  OffsetRect(&CellsPos, ScrollPos->x, ScrollPos->y);

  int DataSize = sizeof(RECT) +
    sizeof(LevelCell) * (CellsPos.right - CellsPos.left) *
      (CellsPos.bottom - CellsPos.top);
  hClipboardData = GlobalAlloc(GMEM_DDESHARE | GMEM_MOVEABLE, DataSize);
  BYTE* RawData = (BYTE*)GlobalLock(hClipboardData);
  memcpy(RawData, &CellsPos, sizeof(RECT));
  LevelCell* Data = (LevelCell*)(RawData + sizeof(RECT));

  for (int y = CellsPos.top; y < CellsPos.bottom; y++)
  {
    for (int x = CellsPos.left; x < CellsPos.right; x++)
    {
      int index = x - CellsPos.left +
        (y - CellsPos.top) * (CellsPos.right - CellsPos.left);
      Data[index] = FloatingSelection[index];
    }
  }

  GlobalUnlock(hClipboardData);

  SetClipboardData(CLIPBOARDFORMAT_ID, hClipboardData);
  CloseClipboard();
  CheckClipboardDataAvailability(hWndCB);
}

BOOL SelectCopyPasteClass::Paste(
  HWND hWndCB,
  POINT* ScrollPos,
  POINT* LastHoveredTile,
  Duke2Map* Map)
{
  BOOL Res = FALSE;

  RECT CellsPos;

  OpenClipboard(hWndCB);
  HGLOBAL hData = GetClipboardData(CLIPBOARDFORMAT_ID);

  BYTE* RawData = (BYTE*)GlobalLock(hData);

  memcpy(&CellsPos, RawData, sizeof(RECT));

  POINT CellsPosSize = {
    CellsPos.right - CellsPos.left, CellsPos.bottom - CellsPos.top};
  SetRect(
    &CellsPos,
    LastHoveredTile->x,
    LastHoveredTile->y,
    LastHoveredTile->x + CellsPosSize.x,
    LastHoveredTile->y + CellsPosSize.y);
  OffsetRect(&CellsPos, -ScrollPos->x, -ScrollPos->y);

  if (FloatingSelection)
  {
    Res = PasteFromFloatingSelection(Map);
    delete FloatingSelection;
  }

  FloatingSelection = new LevelCell[CellsPosSize.x * CellsPosSize.y];
  LevelCell* Data = (LevelCell*)(RawData + sizeof(RECT));

  for (int y = CellsPos.top; y < CellsPos.bottom; y++)
  {
    for (int x = CellsPos.left; x < CellsPos.right; x++)
    {
      int index = x - CellsPos.left + (y - CellsPos.top) * CellsPosSize.x;
      FloatingSelection[index] = Data[index];
    }
  }

  GlobalUnlock(hClipboardData);
  CloseClipboard();
  SelectionActive = TRUE;
  SetRect(
    &FocusRect,
    CellsPos.left * TILESIZE_EDITOR,
    CellsPos.top * TILESIZE_EDITOR,
    CellsPos.right * TILESIZE_EDITOR,
    CellsPos.bottom * TILESIZE_EDITOR);
  FloatingSelDst = FocusRect;
  StartScrollPos = *ScrollPos;
  FloatingSelIsFromClipboard = TRUE;
  OffsetRect(
    &FloatingSelDst,
    ScrollPos->x * TILESIZE_EDITOR,
    ScrollPos->y * TILESIZE_EDITOR);

  return Res;
}

void SelectCopyPasteClass::Delete(POINT* ScrollPos, Duke2Map* Level)
{
  RECT CellsPos = {
    FocusRect.left / TILESIZE_EDITOR,
    FocusRect.top / TILESIZE_EDITOR,
    FocusRect.right / TILESIZE_EDITOR,
    FocusRect.bottom / TILESIZE_EDITOR};
  LevelCell Cell;

  POINT CellsPosSize = {
    CellsPos.right - CellsPos.left, CellsPos.bottom - CellsPos.top};

  delete DelUndoBefore;
  delete DelUndoAfter;
  DelUndoBefore = new LevelCell[CellsPosSize.x * CellsPosSize.y];
  DelUndoAfter = new LevelCell[CellsPosSize.x * CellsPosSize.y];

  OffsetRect(&CellsPos, ScrollPos->x, ScrollPos->y);

  for (int y = CellsPos.top; y < CellsPos.bottom; y++)
  {
    for (int x = CellsPos.left; x < CellsPos.right; x++)
    {
      POINT Pos = {x, y};
      Level->GetCellAttributes(&Pos, &Cell);
      DelUndoBefore[x - CellsPos.left + (y - CellsPos.top) * CellsPosSize.x] =
        Cell;
      Level->SetCellAttributes(
        &Pos, Cell.Solid = 0, Cell.Masked = 0, Cell.bMasked = FALSE);
      DelUndoAfter[x - CellsPos.left + (y - CellsPos.top) * CellsPosSize.x] =
        Cell;
    }
  }

  SelectionActive = NULL != FloatingSelection;
}
