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


#ifndef SELECTCOPYPASTE_H
#define SELECTCOPYPASTE_H

#include "Constants.h"
#include "Map.h"
#include <windows.h>

const int CLIPBOARDFORMAT_ID = CF_PRIVATEFIRST + 1;

extern class SelectCopyPasteClass
{
private:
  HGLOBAL hClipboardData;
  HDC hDCFocus;
  POINT FocusOrigin, StartScrollPos;
  BOOL Dragging, SelectionActive, MovingSelection, FloatingSelIsFromClipboard;
  RECT FocusRect, FloatingSelDst, FloatingSelOrigin;
  LevelCell *FloatingSelection, *DelUndoBefore, *DelUndoAfter;

public:
  ~SelectCopyPasteClass()
  {
    delete FloatingSelection;
    delete DelUndoBefore;
    delete DelUndoAfter;
  }

  friend void DrawFloatingSelection(SelectCopyPasteClass*, POINT*);

  BOOL FocusRectChange(POINT* Pos);
  void KillFloatingSelection(void);
  void Deselect(POINT*, Duke2Map*);
  BOOL SelectionFinished(HWND, POINT*, Duke2Map*, BOOL* Pasted);
  BOOL PasteFromFloatingSelection(Duke2Map*);
  void CopyToFloatingSelection(POINT*, Duke2Map*);
  BOOL Paste(HWND, POINT*, POINT*, Duke2Map*);
  void Copy(HWND, POINT*);
  void Delete(POINT*, Duke2Map*);

  void OnDestroyClipboard(void)
  {
    GlobalFree(hClipboardData);
    hClipboardData = NULL;
  }

  void BeginFocusDragging(POINT* Pos, POINT* ScrollPos, HWND hWndFD);
  void EndFocusDragging(HWND hWndFD)
  {
    Dragging = FALSE;
    DrawFocusRect(hDCFocus, &FocusRect);
    ReleaseDC(hWndFD, hDCFocus);
  }

  void MoveSelection(POINT* Pos, POINT* LastHoveredTile)
  {
    if (MovingSelection)
      OffsetRect(
        &FocusRect,
        (Pos->x - LastHoveredTile->x) * TILESIZE_EDITOR,
        (Pos->y - LastHoveredTile->y) * TILESIZE_EDITOR);
  }

  BOOL CheckClipboardDataAvailability(HWND hWndCB)
  {
    OpenClipboard(hWndCB);
    BOOL Res = CLIPBOARDFORMAT_ID == EnumClipboardFormats(0);
    CloseClipboard();

    return Res;
  }

  BOOL IsSelectionActive(void) { return SelectionActive; }
  BOOL IsFloatingSelFromClipboard(void) { return FloatingSelIsFromClipboard; }
  BOOL IsThereFloatingSel(void) { return (FloatingSelection != NULL); }
  BOOL IsDragging(void) { return Dragging; }
  BOOL PointInSelection(POINT* pPos, POINT* ScrollPos)
  {
    POINT Pos = {
      pPos->x + ScrollPos->x * TILESIZE_EDITOR,
      pPos->y + ScrollPos->y * TILESIZE_EDITOR};
    return SelectionActive && PtInRect(&FloatingSelDst, Pos) || MovingSelection;
  }
  void GetFloatingSelRect(RECT* r) { *r = FocusRect; }
} SelectCopyPaste;

#endif