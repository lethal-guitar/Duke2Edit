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


#include "Gui.h"
#include "Duke2Files.h"
#include "EditorGFX.h"
#include "Messages.h"

GuiClass Gui;

void GuiClass::SizeStatusbarParts(void)
{
  RECT r;
  GetClientRect(hWnd, &r);

  int Sizes[NR_STATUSBAR_PARTS] = {
    r.right - 360, r.right - 310, r.right - 270, r.right - 230, r.right};

  SendMessage(hWndStatusBar, SB_SETPARTS, NR_STATUSBAR_PARTS, LPARAM(Sizes));
}

BOOL GuiClass::Init(void)
{
  InitCommonControls();

  hWndStatusBar = CreateStatusWindow(
    SBARS_SIZEGRIP | WS_CHILD | WS_VISIBLE, NULL, hWnd, STATUSBAR_ID);

  if (!hWndStatusBar)
  {
    Error("GuiClass::Init(): Failed to create status bar!");
    return FALSE;
  }

  SizeStatusbarParts();

  TBBUTTON TBButtons[NR_TOOLBAR_BTNS];
  ZeroMemory(TBButtons, sizeof(TBBUTTON) * NR_TOOLBAR_BTNS);

  TBButtons[3].fsStyle = TBButtons[7].fsStyle = TBButtons[10].fsStyle =
    TBButtons[14].fsStyle = TBButtons[15].fsStyle = TBSTYLE_SEP;
  
  int i;
  for (i = 0; i < 3; i++)
  {
    TBButtons[i].iBitmap = i + 1;
    TBButtons[i].fsState = TBSTATE_ENABLED;
  }

  for (i = 4; i < 7; i++)
    TBButtons[i].iBitmap = i;

  for (i = 8; i < 10; i++)
    TBButtons[i].iBitmap = i - 1;

  for (i = 11; i < 15; i++)
  {
    TBButtons[i].fsStyle = TBSTYLE_GROUP | TBSTYLE_CHECK;
    TBButtons[i].fsState = TBSTATE_ENABLED;
    TBButtons[i].iBitmap = i - 2;
  }

  TBButtons[16].iBitmap = 13;
  TBButtons[16].fsState = TBSTATE_ENABLED;

  TBButtons[11].fsState |= TBSTATE_CHECKED;

  TBButtons[0].idCommand = ID_NEWFILE;
  TBButtons[1].idCommand = ID_OPENFILE;
  TBButtons[2].idCommand = ID_SAVEFILE;
  TBButtons[4].idCommand = ID_CUT;
  TBButtons[5].idCommand = ID_COPY;
  TBButtons[6].idCommand = ID_PASTE;
  TBButtons[8].idCommand = ID_UNDO;
  TBButtons[9].idCommand = ID_REDO;
  TBButtons[11].idCommand = ID_PLACESOLID;
  TBButtons[12].idCommand = ID_PLACEMASKED;
  TBButtons[13].idCommand = ID_PLACEACTORS;
  TBButtons[14].idCommand = ID_SELECT;
  TBButtons[16].idCommand = ID_LEVELPROPS;

  hWndToolbar = CreateToolbarEx(
    hWnd,
    WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT,
    TOOLBAR_ID,
    NR_TOOLBAR_BTNS + 1,
    hInstance,
    IDR_MAINTOOLBAR,
    TBButtons,
    NR_TOOLBAR_BTNS,
    16,
    15,
    24,
    22,
    sizeof(TBBUTTON));

  if (!hWndToolbar)
  {
    Error("GuiClass::Init(): Failed to create toolbar!");
    return FALSE;
  }

  RECT r;
  GetClientRect(hWndToolbar, &r);
  Toolbar_Height = r.bottom - r.top;

  GetWindowRect(hWndStatusBar, &r);
  StatusBar_Height = r.bottom - r.top;

  WNDCLASSEX wc;
  ZeroMemory(&wc, sizeof(WNDCLASSEX));

  wc.cbSize = sizeof(WNDCLASSEX);
  wc.hInstance = hInstance;
  wc.lpfnWndProc = DAWindowProc;
  wc.hbrBackground = NULL;
  wc.hCursor = NULL;
  wc.lpszMenuName = NULL;
  wc.lpszClassName = DRAWAREA_WNDCLASSNAME;

  if (!RegisterClassEx(&wc))
  {
    Error("GuiClass::Init(): Failed to register draw area window class!");
    return FALSE;
  }

  SetRect(&r, 0, 0, 640, 64);
  AdjustWindowRectEx(&r, WS_CHILD, FALSE, WS_EX_CLIENTEDGE);

  MaskedTiles_Height = r.bottom - r.top;

  SetRect(&r, 0, 0, 640, 128);
  AdjustWindowRectEx(
    &r, WS_CHILD | WS_HSCROLL | WS_VSCROLL, FALSE, WS_EX_CLIENTEDGE);

  SolidTiles_Height = r.bottom - r.top;

  DrawArea_HeightOffset = StatusBar_Height + MaskedTiles_Height +
    SolidTiles_Height + Toolbar_Height + 1;

  GetClientRect(hWnd, &r);

  hWndDrawArea = CreateWindowEx(
    WS_EX_CLIENTEDGE,
    DRAWAREA_WNDCLASSNAME,
    NULL,
    WS_HSCROLL | WS_VSCROLL | WS_CHILD | WS_VISIBLE,
    0,
    Toolbar_Height,
    r.right,
    r.bottom - DrawArea_HeightOffset,
    hWnd,
    NULL,
    hInstance,
    NULL);

  if (!hWndDrawArea)
  {
    Error("GuiClass::Init(): Failed to create draw area window!");
    return FALSE;
  }

  wc.lpfnWndProc = TSWindowProc;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.lpszClassName = SOLIDTILES_WNDCLASSNAME;

  if (!RegisterClassEx(&wc))
  {
    Error(
      "GuiClass::Init(): Failed to register solid tile selection window class!");
    return FALSE;
  }

  SetRect(&r, 0, 0, 640 + GetSystemMetrics(SM_CXVSCROLL), 128);
  AdjustWindowRectEx(&r, WS_CHILD | WS_VSCROLL, FALSE, WS_EX_CLIENTEDGE);

  RECT r2 = {0};
  GetClientRect(hWnd, &r2);

  hWndSolidTiles = CreateWindowEx(
    WS_EX_CLIENTEDGE,
    SOLIDTILES_WNDCLASSNAME,
    "Solid Tiles",
    WS_CHILD | WS_VISIBLE | WS_VSCROLL,
    0,
    r2.bottom - StatusBar_Height - MaskedTiles_Height - SolidTiles_Height,
    r.right - r.left,
    r.bottom - r.top,
    hWnd,
    NULL,
    hInstance,
    NULL);

  if (!hWndSolidTiles)
  {
    Error("GuiClass::Init(): Failed to create solid tile selection window!");
    return FALSE;
  }

  wc.lpfnWndProc = TSWindowProc;
  wc.lpszClassName = MASKEDTILES_WNDCLASSNAME;

  if (!RegisterClassEx(&wc))
  {
    Error(
      "GuiClass::Init(): Failed to register masked tile selection window class!");
    return FALSE;
  }

  SetRect(&r, 0, 0, 640, 64);
  AdjustWindowRectEx(&r, WS_CHILD, FALSE, WS_EX_CLIENTEDGE);

  hWndMaskedTiles = CreateWindowEx(
    WS_EX_CLIENTEDGE,
    MASKEDTILES_WNDCLASSNAME,
    "Masked Tiles",
    WS_CHILD | WS_VISIBLE,
    0,
    r2.bottom - StatusBar_Height - MaskedTiles_Height + 1,
    r.right - r.left,
    r.bottom - r.top,
    hWnd,
    NULL,
    hInstance,
    NULL);

  if (!hWndMaskedTiles)
  {
    Error("GuiClass::Init(): Failed to create masked tile selection window!");
    return FALSE;
  }

  GetClientRect(hWndDrawArea, &DAClientRect);

  ZeroMemory(&ScrollInfo, sizeof(SCROLLINFO));
  ScrollInfo.cbSize = sizeof(SCROLLINFO);

  InitScrollBars(
    hWndDrawArea,
    DAClientRect.right / TILESIZE_EDITOR,
    DAClientRect.bottom / TILESIZE_EDITOR,
    START_LEVELWIDTH - 1,
    LEVEL_DATA_WORDS / START_LEVELWIDTH - 1);

  ScrollInfo.nMax = 400 / TILESIZE_EDITOR - 1;
  ScrollInfo.nPage = STPage = 128 / TILESIZE_EDITOR;

  SetScrollInfo(hWndSolidTiles, SB_VERT, &ScrollInfo, TRUE);

  Page.x = DAClientRect.right / TILESIZE_EDITOR;
  Page.y = DAClientRect.bottom / TILESIZE_EDITOR;

  hMenuView = GetSubMenu(GetMenu(hWnd), 2);
  hMenuPopup =
    GetSubMenu(hMenuP = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_POPUP)), 0);

  CheckMenuRadioItem(
    hMenuPopup, ID_PLACESOLID, ID_SELECT, ID_PLACESOLID, MF_BYCOMMAND);
  hCursorDrawTile = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_DRAWTILE));
  hCursorStd = LoadCursor(NULL, IDC_ARROW);
  hCursorMove = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_MOVE));

  return TRUE;
}

void GuiClass::InitScrollBars(
  HWND hWndSBar,
  UINT PageHorz,
  UINT PageVert,
  int MaxHorz,
  int MaxVert)
{
  ZeroMemory(&ScrollInfo, sizeof(SCROLLINFO));
  ScrollInfo.cbSize = sizeof(SCROLLINFO);

  ScrollPos.x = ScrollPos.y = 0;

  ScrollInfo.fMask = SIF_ALL;
  ScrollInfo.nMax = MaxHorz;
  ScrollInfo.nPage = PageHorz;

  SetScrollInfo(hWndSBar, SB_HORZ, &ScrollInfo, TRUE);

  ScrollInfo.nMax = MaxVert;
  ScrollInfo.nPage = PageVert;

  SetScrollInfo(hWndSBar, SB_VERT, &ScrollInfo, TRUE);
}

void GuiClass::SetEditMode(EDITMODE Em)
{
  EditMode = Em;

  CheckMenuRadioItem(
    hMenuPopup,
    ID_PLACESOLID,
    ID_SELECT,
    ID_PLACESOLID + EditMode,
    MF_BYCOMMAND);
  SendMessage(hWndToolbar, TB_CHECKBUTTON, EditMode + 40041, MAKELONG(TRUE, 0));

  switch (EditMode)
  {
    case EM_PLACESOLIDTILES:
    case EM_PLACEMASKEDTILES:
      hDACursor = hCursorDrawTile;
      break;

    default:
      hDACursor = hCursorStd;
  }
}

void GuiClass::OnSTVertScroll(HWND hWndSBar, WPARAM wParam, LPARAM lParam)
{
  switch (LOWORD(wParam))
  {
    case SB_LINEUP:
      ScrollInfo.fMask = SIF_POS;
      GetScrollInfo(hWndSBar, SB_VERT, &ScrollInfo);

      ScrollInfo.nPos--;

      SetScrollInfo(hWndSBar, SB_VERT, &ScrollInfo, TRUE);
      GetScrollInfo(hWndSBar, SB_VERT, &ScrollInfo);

      STPos = ScrollInfo.nPos;
      break;

    case SB_LINEDOWN:
      ScrollInfo.fMask = SIF_POS;
      GetScrollInfo(hWndSBar, SB_VERT, &ScrollInfo);

      ScrollInfo.nPos++;

      SetScrollInfo(hWndSBar, SB_VERT, &ScrollInfo, TRUE);
      GetScrollInfo(hWndSBar, SB_VERT, &ScrollInfo);

      STPos = ScrollInfo.nPos;
      break;

    case SB_PAGEUP:
      ScrollInfo.fMask = SIF_POS | SIF_PAGE;
      GetScrollInfo(hWndSBar, SB_VERT, &ScrollInfo);

      ScrollInfo.nPos -= ScrollInfo.nPage;
      ScrollInfo.fMask = SIF_POS;

      SetScrollInfo(hWndSBar, SB_VERT, &ScrollInfo, TRUE);
      GetScrollInfo(hWndSBar, SB_VERT, &ScrollInfo);

      STPos = ScrollInfo.nPos;
      break;

    case SB_PAGEDOWN:
      ScrollInfo.fMask = SIF_POS | SIF_PAGE;
      GetScrollInfo(hWndSBar, SB_VERT, &ScrollInfo);

      ScrollInfo.nPos += ScrollInfo.nPage;
      ScrollInfo.fMask = SIF_POS;

      SetScrollInfo(hWndSBar, SB_VERT, &ScrollInfo, TRUE);
      GetScrollInfo(hWndSBar, SB_VERT, &ScrollInfo);

      STPos = ScrollInfo.nPos;
      break;

    case SB_THUMBTRACK:
      ScrollInfo.fMask = SIF_TRACKPOS;
      GetScrollInfo(hWndSBar, SB_VERT, &ScrollInfo);

      ScrollInfo.nPos = STPos = ScrollInfo.nTrackPos;
      ScrollInfo.fMask = SIF_POS;

      SetScrollInfo(hWndSBar, SB_VERT, &ScrollInfo, TRUE);
      break;
  }

  // RedrawSolidTilesWin();
  RedrawWindow(hWndSolidTiles);
}

void GuiClass::OnVertScroll(HWND hWndSBar, WPARAM wParam, LPARAM lParam)
{
  switch (LOWORD(wParam))
  {
    case SB_LINEUP:
      ScrollInfo.fMask = SIF_POS;
      GetScrollInfo(hWndSBar, SB_VERT, &ScrollInfo);

      ScrollInfo.nPos--;

      SetScrollInfo(hWndSBar, SB_VERT, &ScrollInfo, TRUE);
      GetScrollInfo(hWndSBar, SB_VERT, &ScrollInfo);

      ScrollPos.y = ScrollInfo.nPos;
      break;

    case SB_LINEDOWN:
      ScrollInfo.fMask = SIF_POS;
      GetScrollInfo(hWndSBar, SB_VERT, &ScrollInfo);

      ScrollInfo.nPos++;

      SetScrollInfo(hWndSBar, SB_VERT, &ScrollInfo, TRUE);
      GetScrollInfo(hWndSBar, SB_VERT, &ScrollInfo);

      ScrollPos.y = ScrollInfo.nPos;
      break;

    case SB_PAGEUP:
      ScrollInfo.fMask = SIF_POS | SIF_PAGE;
      GetScrollInfo(hWndSBar, SB_VERT, &ScrollInfo);

      ScrollInfo.nPos -= ScrollInfo.nPage;
      ScrollInfo.fMask = SIF_POS;

      SetScrollInfo(hWndSBar, SB_VERT, &ScrollInfo, TRUE);
      GetScrollInfo(hWndSBar, SB_VERT, &ScrollInfo);

      ScrollPos.y = ScrollInfo.nPos;
      break;

    case SB_PAGEDOWN:
      ScrollInfo.fMask = SIF_POS | SIF_PAGE;
      GetScrollInfo(hWndSBar, SB_VERT, &ScrollInfo);

      ScrollInfo.nPos += ScrollInfo.nPage;
      ScrollInfo.fMask = SIF_POS;

      SetScrollInfo(hWndSBar, SB_VERT, &ScrollInfo, TRUE);
      GetScrollInfo(hWndSBar, SB_VERT, &ScrollInfo);

      ScrollPos.y = ScrollInfo.nPos;
      break;

    case SB_THUMBTRACK:
      ScrollInfo.fMask = SIF_TRACKPOS;
      GetScrollInfo(hWndSBar, SB_VERT, &ScrollInfo);

      ScrollInfo.nPos = ScrollPos.y = ScrollInfo.nTrackPos;
      ScrollInfo.fMask = SIF_POS;

      SetScrollInfo(hWndSBar, SB_VERT, &ScrollInfo, TRUE);
      break;
  }

  RedrawLevel();
}

void GuiClass::OnHorizScroll(HWND hWndSBar, WPARAM wParam, LPARAM lParam)
{
  switch (LOWORD(wParam))
  {
    case SB_LINELEFT:
      ScrollInfo.fMask = SIF_POS;
      GetScrollInfo(hWndSBar, SB_HORZ, &ScrollInfo);

      ScrollInfo.nPos--;

      SetScrollInfo(hWndSBar, SB_HORZ, &ScrollInfo, TRUE);
      GetScrollInfo(hWndSBar, SB_HORZ, &ScrollInfo);

      ScrollPos.x = ScrollInfo.nPos;
      break;

    case SB_LINERIGHT:
      ScrollInfo.fMask = SIF_POS;
      GetScrollInfo(hWndSBar, SB_HORZ, &ScrollInfo);

      ScrollInfo.nPos++;

      SetScrollInfo(hWndSBar, SB_HORZ, &ScrollInfo, TRUE);
      GetScrollInfo(hWndSBar, SB_HORZ, &ScrollInfo);

      ScrollPos.x = ScrollInfo.nPos;
      break;

    case SB_PAGELEFT:
      ScrollInfo.fMask = SIF_POS | SIF_PAGE;
      GetScrollInfo(hWndSBar, SB_HORZ, &ScrollInfo);

      ScrollInfo.nPos -= ScrollInfo.nPage;
      ScrollInfo.fMask = SIF_POS;

      SetScrollInfo(hWndSBar, SB_HORZ, &ScrollInfo, TRUE);
      GetScrollInfo(hWndSBar, SB_HORZ, &ScrollInfo);

      ScrollPos.x = ScrollInfo.nPos;
      break;

    case SB_PAGERIGHT:
      ScrollInfo.fMask = SIF_POS | SIF_PAGE;
      GetScrollInfo(hWndSBar, SB_HORZ, &ScrollInfo);

      ScrollInfo.nPos += ScrollInfo.nPage;
      ScrollInfo.fMask = SIF_POS;

      SetScrollInfo(hWndSBar, SB_HORZ, &ScrollInfo, TRUE);
      GetScrollInfo(hWndSBar, SB_HORZ, &ScrollInfo);

      ScrollPos.x = ScrollInfo.nPos;
      break;

    case SB_THUMBTRACK:
      ScrollInfo.fMask = SIF_TRACKPOS;
      GetScrollInfo(hWndSBar, SB_HORZ, &ScrollInfo);

      ScrollInfo.nPos = ScrollPos.x = ScrollInfo.nTrackPos;
      ScrollInfo.fMask = SIF_POS;

      SetScrollInfo(hWndSBar, SB_HORZ, &ScrollInfo, TRUE);
      break;
  }

  RedrawLevel();
}