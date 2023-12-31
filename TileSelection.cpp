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


#include "TileSelection.h"
#include "EditorGFX.h"
#include "Gui.h"

static BOOL Dragging;
static RECT FocusRect;
static POINT FocusOrigin;
static HDC hDCFocus;
static HWND hWndDC, hWndLButton;

void OnTSMouseMove(HWND hWndMM, WPARAM wParam, LPARAM lParam)
{
  POINT Pos = {
    LOWORD(lParam) / TILESIZE_EDITOR, HIWORD(lParam) / TILESIZE_EDITOR};

  if (Gui.IsSolidTilesWin(hWndMM))
    Pos.y += Gui.GetSTScrollPos();

  char Str[128];
  sprintf(Str, "%d", Pos.x + Pos.y * 40);
  Gui.SetStatusText(1, Str);
  Gui.SetStatusText(2, NULL);
  Gui.SetStatusText(3, NULL);

  GetTileAttributesString(
    Pos.x + Pos.y * 40 + ((!Gui.IsSolidTilesWin(hWndMM)) ? 1000 : 0), Str);
  Gui.SetStatusText(4, Str);

  if (Dragging && hWndMM == hWndLButton)
  {
    Pos.x *= TILESIZE_EDITOR;
    Pos.y *= TILESIZE_EDITOR;

    DrawFocusRect(hDCFocus, &FocusRect);

    FocusRect.left = FocusOrigin.x;
    FocusRect.top = FocusOrigin.y;
    FocusRect.right = Pos.x + TILESIZE_EDITOR;
    FocusRect.bottom = Pos.y + TILESIZE_EDITOR;

    if (Gui.IsSolidTilesWin(hWndMM))
      FocusRect.bottom -= Gui.GetSTScrollPos() * TILESIZE_EDITOR;

    if (Pos.x < FocusRect.left)
    {
      FocusRect.left = FocusRect.right - TILESIZE_EDITOR;
      FocusRect.right = FocusOrigin.x;
    }

    if (Pos.y < FocusRect.top)
    {
      FocusRect.top = FocusRect.bottom - TILESIZE_EDITOR;
      FocusRect.bottom = FocusOrigin.y;
    }

    DrawFocusRect(hDCFocus, &FocusRect);
  }
}

void OnTSLButtonUp(HWND hWndLocal, WPARAM wParam, LPARAM lParam)
{
  if (hWndLocal != hWndLButton)
  {
    EndTSFocusDragging();
    return;
  }

  POINT Pos = {
    FocusRect.left / TILESIZE_EDITOR, FocusRect.top / TILESIZE_EDITOR};

  if (Gui.IsSolidTilesWin(hWndLocal))
  {
    BrushSizeSolid.x = (FocusRect.right - FocusRect.left) / TILESIZE_EDITOR +
      ((FocusRect.right - FocusRect.left) ? 0 : 1);
    BrushSizeSolid.y = (FocusRect.bottom - FocusRect.top) / TILESIZE_EDITOR +
      ((FocusRect.bottom - FocusRect.top) ? 0 : 1);
    BrushSize = BrushSizeSolid;
    CurrentSolidTile = Pos.x + (Pos.y + Gui.GetSTScrollPos()) * 40;
  }
  else
  {
    BrushSizeMasked.x = (FocusRect.right - FocusRect.left) / TILESIZE_EDITOR +
      ((FocusRect.right - FocusRect.left) ? 0 : 1);
    BrushSizeMasked.y = (FocusRect.bottom - FocusRect.top) / TILESIZE_EDITOR +
      ((FocusRect.bottom - FocusRect.top) ? 0 : 1);
    BrushSize = BrushSizeMasked;
    CurrentMaskedTile = Pos.x + Pos.y * 40;
  }

  Gui.RedrawCZone();

  ReleaseDC(hWndDC, hDCFocus);
  hDCFocus = NULL;
  Dragging = FALSE;
  hWndLButton = NULL;
}

void OnTSLButtonDown(HWND hWndLocal, WPARAM wParam, LPARAM lParam)
{
  POINT Pos = {
    LOWORD(lParam) / TILESIZE_EDITOR * TILESIZE_EDITOR,
    HIWORD(lParam) / TILESIZE_EDITOR * TILESIZE_EDITOR};

  Dragging = TRUE;
  SetRect(&FocusRect, Pos.x, Pos.y, Pos.x, Pos.y);
  FocusOrigin.x = Pos.x;
  FocusOrigin.y = Pos.y;
  hDCFocus = GetDC(hWndDC = hWndLocal);
  DrawFocusRect(hDCFocus, &FocusRect);
  hWndLButton = hWndLocal;
}

void EndTSFocusDragging(void)
{
  Dragging = FALSE;
  DrawFocusRect(hDCFocus, &FocusRect);
  ReleaseDC(hWndDC, hDCFocus);
  hDCFocus = NULL;
}