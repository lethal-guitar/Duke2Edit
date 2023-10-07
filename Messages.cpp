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


/******************************************************

        Messages.cpp

******************************************************/

#pragma warning(disable : 4786)

#include "Messages.h"
#include "CMP.h"
#include "D3D.h"
#include "EditorGFX.h"
#include "Gui.h"
#include "LevelPropsDialog.h"
#include "Map.h"
#include "MapOverview.h"
#include "SelectCopyPaste.h"
#include "Shared.h"
#include "TileSelection.h"
#include "Undoredo.h"
#include "resource.h"
#include <commctrl.h>
#include <list>
#include <string>
#include <strstream>

// -------------------------------------------------------
// Constants
// -------------------------------------------------------
const char START_CMP[] = "nukem2.cmp";
const char FILENAME_FILTER[] = "Duke Nukem II Levels (*.mni)\0*.mni\0";

// -------------------------------------------------------
// Variables
// -------------------------------------------------------
static POINT ActorChangePos, LastHoveredTile;
static BOOL D3D_Ready, FileChanged, ActorHovered, LButtonPressed,
  GrayedOutLastMasked;
static string CurrentFileName;
static HPEN hPen;
static int DialogType;
static HBITMAP hBmCZone, hBmBackdrop, hBmActorPreview;
static list<string> LevelList, CZoneList, BackdropList, MusicList;
static HWND hWndEditOpsDlg, hWndLevelOverviewDlg;

// -------------------------------------------------------
// Prototypes
// -------------------------------------------------------
static void OnVertScroll(HWND, WPARAM, LPARAM);
static void OnHorizScroll(HWND, WPARAM, LPARAM);
static void OnCommand(HWND, WPARAM, LPARAM);
static void OnPaint(HWND, WPARAM, LPARAM);
static void OnTSPaint(HWND, WPARAM, LPARAM);
static void OnSize(HWND, WPARAM, LPARAM);
static void OnLButtonUp(HWND, WPARAM, LPARAM);
static void OnLButtonDown(HWND, WPARAM, LPARAM);
static void OnMouseMove(HWND, WPARAM, LPARAM);
static void OnNCMouseMove(HWND, WPARAM, LPARAM);
static void OnSTVertScroll(HWND, WPARAM, LPARAM);
static BOOL OnContextMenu(HWND, WPARAM, LPARAM);
static void OnInitDialog(HWND, WPARAM, LPARAM);
static void DialogOnCommand(HWND, WPARAM, LPARAM);
static void OnNotify(HWND, WPARAM, LPARAM);

static LRESULT OnCreate(HWND, WPARAM, LPARAM);

static BOOL CheckIfFileWasSaved(void);
static void NewFileOpened(string NewCurrentFileName, BOOL = TRUE);
static void CreateNameLists(void);
static void ChangeEditMode(EDITMODE);
inline void ChangeCZone(const char* Name);
inline void IndicateFileChange(void);

// -------------------------------------------------------
// 					 Callback functions
// -------------------------------------------------------
LRESULT CALLBACK
  MainWindowProc(HWND hWndLocal, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg)
  {
    case WM_MOUSEMOVE:
    case WM_NCMOUSEMOVE:
      Gui.MouseLeftDrawArea();
      break;

    case WM_LBUTTONUP:
    case WM_NCLBUTTONUP:
      SelectCopyPaste.EndFocusDragging(Gui.GetDrawAreaWindow());
      EndTSFocusDragging();
      break;

    case WM_ACTIVATE:
      if (WA_ACTIVE == LOWORD(wParam) || WA_CLICKACTIVE == LOWORD(wParam))
        Gui.EnablePaste(SelectCopyPaste.CheckClipboardDataAvailability(
          Gui.GetDrawAreaWindow()));

      return DefWindowProc(hWndLocal, msg, wParam, lParam);
      break;

    case WM_CONTEXTMENU:
      if (!OnContextMenu(hWndLocal, wParam, lParam))
        return DefWindowProc(hWndLocal, msg, wParam, lParam);
      break;

    case WM_CREATE:
      return OnCreate(hWndLocal, wParam, lParam);
      break;

    case WM_COMMAND:
      OnCommand(hWndLocal, wParam, lParam);
      break;

    case WM_SIZE:
      OnSize(hWndLocal, wParam, lParam);
      break;

    case WM_NOTIFY:
      OnNotify(hWndLocal, wParam, lParam);
      break;

    case WM_CLOSE:
      if (CheckIfFileWasSaved())
        DestroyWindow(hWnd);
      break;

    case WM_DESTROY:
      PostQuitMessage(TRUE);
      break;

    default:
      return DefWindowProc(hWndLocal, msg, wParam, lParam);
  }

  return 0;
}

LRESULT CALLBACK
  DAWindowProc(HWND hWndLocal, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg)
  {
    case WM_NCMOUSEMOVE:
      Gui.MouseLeftDrawArea();
      break;

    case WM_DESTROYCLIPBOARD:
      SelectCopyPaste.OnDestroyClipboard();
      break;

    case WM_LBUTTONUP:
      OnLButtonUp(hWndLocal, wParam, lParam);
      // fallthrough

    case WM_NCLBUTTONUP:
      SelectCopyPaste.EndFocusDragging(Gui.GetDrawAreaWindow());
      EndTSFocusDragging();
      break;

    case WM_LBUTTONDOWN:
      OnLButtonDown(hWndLocal, wParam, lParam);
      break;

    case WM_MOUSEMOVE:
      OnMouseMove(hWndLocal, wParam, lParam);
      break;

    case WM_PAINT:
      OnPaint(hWndLocal, wParam, lParam);
      break;

    case WM_VSCROLL:
      Gui.OnVertScroll(hWndLocal, wParam, lParam);
      // RedrawWindow(hWndLevelOverviewDlg, NULL, NULL, RDW_INVALIDATE);
      break;

    case WM_HSCROLL:
      Gui.OnHorizScroll(hWndLocal, wParam, lParam);
      // RedrawWindow(hWndLevelOverviewDlg, NULL, NULL, RDW_INVALIDATE);
      break;

    default:
      return DefWindowProc(hWndLocal, msg, wParam, lParam);
  }

  return 0;
}

LRESULT CALLBACK
  TSWindowProc(HWND hWndLocal, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg)
  {
    case WM_MOUSEMOVE:
      OnTSMouseMove(hWndLocal, wParam, lParam);
      Gui.MouseLeftDrawArea();
      break;

    case WM_NCMOUSEMOVE:
      Gui.MouseLeftDrawArea();
      break;

    case WM_PAINT:
      OnTSPaint(hWndLocal, wParam, lParam);
      break;

    case WM_VSCROLL:
      if (Gui.IsSolidTilesWin(hWndLocal))
        Gui.OnSTVertScroll(hWndLocal, wParam, lParam);
      break;

    case WM_LBUTTONUP:
      OnTSLButtonUp(hWndLocal, wParam, lParam);
      ChangeEditMode(
        (Gui.IsSolidTilesWin(hWndLocal)) ? EM_PLACESOLIDTILES
                                         : EM_PLACEMASKEDTILES);
      // fallthrough

    case WM_NCLBUTTONUP:
      SelectCopyPaste.EndFocusDragging(Gui.GetDrawAreaWindow());
      EndTSFocusDragging();
      break;

    case WM_LBUTTONDOWN:
      OnTSLButtonDown(hWndLocal, wParam, lParam);
      break;

    default:
      return DefWindowProc(hWndLocal, msg, wParam, lParam);
  }

  return 0;
}

BOOL CALLBACK DialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg)
  {
    case WM_MOUSEMOVE:
    case WM_NCMOUSEMOVE:
      Gui.MouseLeftDrawArea();
      return FALSE;
      break;

    case WM_LBUTTONUP:
    case WM_NCLBUTTONUP:
      SelectCopyPaste.EndFocusDragging(Gui.GetDrawAreaWindow());
      return FALSE;
      break;

    case WM_INITDIALOG:
      OnInitDialog(hDlg, wParam, DialogType = lParam);
      break;

    case WM_COMMAND:
      DialogOnCommand(hDlg, wParam, lParam);
      break;

    case WM_CLOSE:
      if (hDlg == hWndEditOpsDlg)
      {
        ShowWindow(hDlg, SW_HIDE);
        Gui.CheckViewMenuItem(ID_SHOWEDITOPS);
      }
      else
        EndDialog(hDlg, FALSE);
      break;

    case WM_NOTIFY:
      OnNotify(hDlg, wParam, lParam);
      break;

    default:
      return FALSE;
  }

  return TRUE;
}

// -------------------------------------------------------
// Helper funtions
// -------------------------------------------------------
void ChangeEditMode(EDITMODE Em)
{
  if (SelectCopyPaste.IsSelectionActive())
  {
    SelectCopyPaste.Deselect(&Gui.GetScrollPos(), &Level);
    Gui.EnableCopyDelete(FALSE);
    Gui.RedrawLevel();
  }

  Gui.SetEditMode(Em);

  if (EM_PLACESOLIDTILES == Em)
    BrushSize = BrushSizeSolid;
  else if (EM_PLACEMASKEDTILES == Em)
    BrushSize = BrushSizeMasked;
}

inline void IndicateFileChange(void)
{
  FileChanged = TRUE;
  Gui.EnableUndo(TRUE);
  Gui.EnableRedo(NewUndoChain());
}

BOOL CheckIfFileWasSaved(void)
{
  if (!FileChanged)
    return TRUE;

  int Result = Message(
    "This operation will discard any changes you made to the current file. Do you want to save it now?",
    MB_YESNOCANCEL | MB_ICONWARNING);

  if (IDYES == Result)
  {
    SendMessage(hWnd, WM_COMMAND, ID_SAVEFILE, NULL);
    return !FileChanged;
  }

  return IDNO == Result;
}

void NewFileOpened(string NewCurrentFileName, BOOL CallLevelSizeChanged)
{
  if (CallLevelSizeChanged)
  {
    Gui.LevelSizeChange(Level.GetLevelWidth(), Level.GetLevelHeight());
    CleanUpUndoRedo();
    Gui.EnableUndo(FALSE);
    Gui.EnableRedo(FALSE);
    Gui.InitCZoneScrollbar();

    CurrentSolidTile = CurrentMaskedTile = 0;
    SendDlgItemMessage(hWndEditOpsDlg, IDC_COMBOACTORTYPE, CB_SETCURSEL, 0, 0);
    SendMessage(
      hWndEditOpsDlg,
      WM_COMMAND,
      MAKELONG(IDC_COMBOACTORTYPE, CBN_SELCHANGE),
      LPARAM(GetDlgItem(hWndEditOpsDlg, IDC_COMBOACTORTYPE)));
    Gui.RedrawCZone();
    ChangeEditMode(EM_PLACESOLIDTILES);
  }

  FileChanged = FALSE;

  CurrentFileName = NewCurrentFileName;
  string Str;

  if (!CurrentFileName.size())
    Str = "Unnamed - ";
  else
    Str = CurrentFileName.substr(
            CurrentFileName.rfind('\\') + 1, CurrentFileName.size()) +
      " - ";

  Str += APPNAME;

  Gui.SetMainWindowCaption(Str.c_str());
}

inline void ChangeCZone(const char* Name)
{
  LoadCZone(Name);
  Level.SetCZone(Name);
  Gui.RedrawCZone();
}

void CreateNameLists(void)
{
  string* Names = new string[CMP.GetNrFiles()];
  CMP.GetFileNames(Names);

  for (int i = 0; i < CMP.GetNrFiles(); i++)
  {
    if (CMP.DetermineFileType(Names[i]) == TYPE_LEVEL)
      LevelList.push_back(Names[i]);
    else if (CMP.DetermineFileType(Names[i]) == TYPE_CZONE)
      CZoneList.push_back(Names[i]);
    else if (
      CMP.DetermineFileType(Names[i]) == TYPE_BGDROP &&
      Names[i] != "STATUS.MNI")
      BackdropList.push_back(Names[i]);
    else if (CMP.DetermineFileType(Names[i]) == TYPE_IMF)
      MusicList.push_back(Names[i]);
  }

  delete[] Names;
}

// -------------------------------------------------------
// Message handlers
// -------------------------------------------------------
void OnInitDialog(HWND hDlg, WPARAM wPAram, LPARAM lParam)
{
  switch (DialogType)
  {
    case IDD_CMPSPECIFY:
      /*
      {
              char Str[32] = "";
              DlgDirList(hDlg, Str, IDC_TREEDIRECTORYS, NULL, DDL_DIRECTORY |
      DDL_EXCLUSIVE);
      }*/
      break;

    case IDD_OPENFROMCMP:
      {
        for (list<string>::iterator it = LevelList.begin();
             it != LevelList.end();
             it++)
        {
          SendDlgItemMessage(
            hDlg, IDC_COMBOLEVELS, CB_ADDSTRING, 0, LPARAM((*it).c_str()));
        }

        SendDlgItemMessage(hDlg, IDC_COMBOLEVELS, CB_SETCURSEL, 0, 0);
      }
      break;

    case IDD_EDITOPS:
      {
        int i = 0;

        while (strcmp(ActorTypes[i], "\0"))
        {
          SendDlgItemMessage(
            hDlg, IDC_COMBOACTORTYPE, CB_ADDSTRING, 0, LPARAM(ActorTypes[i]));
          i++;
        }

        SendDlgItemMessage(hDlg, IDC_COMBOACTORTYPE, CB_SETCURSEL, 0, 0);

        i = 0;

        while (strcmp(ActorDefs[i].Name, "\0"))
        {
          if (strcmp(ActorDefs[i].Name, "??"))
            SendDlgItemMessage(
              hDlg,
              IDC_COMBOACTORS,
              CB_ADDSTRING,
              0,
              LPARAM(ActorDefs[i].Name));

          i++;
        }

        LoadMultipleActorImages((char*)ActorDefs[0].ImageId, &hBmActorPreview);

        SendDlgItemMessage(
          hDlg,
          IDC_ACTORPREVIEW,
          STM_SETIMAGE,
          IMAGE_BITMAP,
          LPARAM(hBmActorPreview));
        SendDlgItemMessage(hDlg, IDC_COMBOACTORS, CB_SETCURSEL, 0, 0);
      }
      break;

    case IDD_LEVELINFO:
      SetDlgItemInt(hDlg, IDC_TEXTLVLWIDTH, Level.GetLevelWidth(), FALSE);
      SetDlgItemInt(hDlg, IDC_TEXTLVLHEIGHT2, Level.GetLevelHeight(), FALSE);
      SetDlgItemInt(
        hDlg,
        IDC_TEXTNRTILES,
        Level.GetLevelWidth() * Level.GetLevelHeight(),
        FALSE);
      SetDlgItemInt(hDlg, IDC_TEXTNRSOLIDTILES, Level.GetNrSolidTiles(), FALSE);
      SetDlgItemInt(
        hDlg, IDC_TEXTNRMASKEDTILES, Level.GetNrMaskedTiles(), FALSE);
      SetDlgItemInt(hDlg, IDC_TEXTNRACTORS, Level.GetNrActors(), FALSE);
      break;
  }
}

void DialogOnCommand(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
  switch (LOWORD(wParam))
  {
    case IDOK:
      switch (DialogType)
      {
        case IDD_OPENFROMCMP:
          char SelStr[7];
          GetDlgItemText(hDlg, IDC_COMBOLEVELS, SelStr, 7);

          SelectCopyPaste.Deselect(&Gui.GetScrollPos(), &Level);

          if (!LoadLevel(SelStr, &Level, TRUE))
          {
            Error("Failed to load level %s!", SelStr);
            break;
          }

          ChangeCZone(Level.GetCZone());
          NewFileOpened("", TRUE);
          break;
      }

      EndDialog(hDlg, TRUE);
      break;

    case IDCANCEL:
      EndDialog(hDlg, FALSE);
      break;
  }

  // Check for Notification messages
  switch (HIWORD(wParam))
  {
    case CBN_SELCHANGE:
      switch (LOWORD(wParam))
      {
        case IDC_COMBOACTORTYPE:
          {
            SendDlgItemMessage(hDlg, IDC_COMBOACTORS, CB_RESETCONTENT, 0, 0);

            int Type =
              SendDlgItemMessage(hDlg, IDC_COMBOACTORTYPE, CB_GETCURSEL, 0, 0);
            int i = 0;

            while (strcmp(ActorDefs[i].Name, "\0"))
            {
              if (
                strcmp(ActorDefs[i].Name, "??") &&
                (ActorDefs[i].Type == Type || Type == AT_ALL))
                SendDlgItemMessage(
                  hDlg,
                  IDC_COMBOACTORS,
                  CB_ADDSTRING,
                  0,
                  LPARAM(ActorDefs[i].Name));


              i++;
            }

            SendDlgItemMessage(hDlg, IDC_COMBOACTORS, CB_SETCURSEL, 0, 0);
          }
          // fallthrough

        case IDC_COMBOACTORS:
          {
            char SelStr[128];
            GetDlgItemText(hDlg, IDC_COMBOACTORS, SelStr, 128);

            int ActorNr = 0;

            while (strcmp(ActorDefs[ActorNr].Name, SelStr) &&
                   strcmp(ActorDefs[ActorNr].Name, "\0"))
              ActorNr++;

            CurrentActor = ActorNr;
            ChangeEditMode(EM_PLACEACTORS);
            DeleteObject(hBmActorPreview);

            if (!strcmp(ActorDefs[ActorNr].ImageId, "-"))
            {
              HDC hDC = GetDC(NULL);
              hBmActorPreview = CreateCompatibleBitmap(hDC, 32, 32);
              HDC hDCMem = CreateCompatibleDC(hDC);
              HDC hDCMem2 = CreateCompatibleDC(hDC);
              HBITMAP hBm = (HBITMAP)LoadImage(
                hInstance,
                MAKEINTRESOURCE(IDB_LEVELSYMBOLS),
                IMAGE_BITMAP,
                0,
                0,
                0);

              SelectObject(hDCMem, hBm);
              SelectObject(hDCMem2, hBmActorPreview);
              BitBlt(hDCMem2, 0, 0, 32, 32, hDCMem, 0, 0, SRCCOPY);

              DeleteObject(hBm);
              DeleteDC(hDCMem2);
              DeleteDC(hDCMem);
              ReleaseDC(NULL, hDC);
            }
            else
              LoadMultipleActorImages(
                (char*)ActorDefs[ActorNr].ImageId, &hBmActorPreview);

            SendDlgItemMessage(
              hDlg,
              IDC_ACTORPREVIEW,
              STM_SETIMAGE,
              IMAGE_BITMAP,
              LPARAM(hBmActorPreview));
          }
          break;
      }
      break;
  }
}

void OnNotify(HWND hWndNotify, WPARAM wParam, LPARAM lParam)
{
  switch (LPNMHDR(lParam)->code)
  {
    case NM_CLICK:
      EndTSFocusDragging();
      SelectCopyPaste.EndFocusDragging(Gui.GetDrawAreaWindow());
      break;

    case NM_HOVER:
      Gui.MouseLeftDrawArea();
      break;
  }
}

void OnCommand(HWND hWndCmd, WPARAM wParam, LPARAM lParam)
{
  int Tmp = 0;
  OPENFILENAME of = {0};
  of.lStructSize = sizeof(OPENFILENAME);
  of.hwndOwner = hWnd;

  char FileName[256];
  ZeroMemory(FileName, 256 * sizeof(char));

  switch (LOWORD(wParam))
  {
    case ID_EXIT:
      if (CheckIfFileWasSaved())
        DestroyWindow(hWnd);
      break;

    case ID_OPENFROMCMP:
      if (CheckIfFileWasSaved())
        DialogBoxParam(
          hInstance,
          MAKEINTRESOURCE(IDD_OPENFROMCMP),
          hWnd,
          DialogProc,
          IDD_OPENFROMCMP);
      break;

    case ID_UNDO:
      if (
        SelectCopyPaste.IsThereFloatingSel() &&
        SelectCopyPaste.IsFloatingSelFromClipboard())
      {
        SelectCopyPaste.PasteFromFloatingSelection(&Level);
        SelectCopyPaste.KillFloatingSelection();
      }

      if (!Undo(&Level))
        Gui.EnableUndo(FALSE);

      Gui.EnableRedo(TRUE);
      Gui.RedrawLevel();
      break;

    case ID_REDO:
      if (!Redo(&Level))
        Gui.EnableRedo(FALSE);

      Gui.EnableUndo(TRUE);
      Gui.RedrawLevel();
      break;

    case ID_CUT:
      SelectCopyPaste.Copy(Gui.GetDrawAreaWindow(), &Gui.GetScrollPos());
      SelectCopyPaste.Delete(&Gui.GetScrollPos(), &Level);
      SelectCopyPaste.KillFloatingSelection();
      Gui.EnableCopyDelete(FALSE);
      Gui.EnablePaste(TRUE);
      IndicateFileChange();
      Gui.RedrawLevel();
      break;

    case ID_COPY:
      SelectCopyPaste.Copy(Gui.GetDrawAreaWindow(), &Gui.GetScrollPos());
      Gui.EnablePaste(TRUE);
      break;

    case ID_PASTE:
      ChangeEditMode(EM_SELECTDATA);
      SelectCopyPaste.Paste(
        Gui.GetDrawAreaWindow(), &Gui.GetScrollPos(), &LastHoveredTile, &Level);
      IndicateFileChange();
      Gui.EnableCopyDelete(TRUE);
      Gui.RedrawLevel();
      break;

    case ID_DELETE:
      if (!SelectCopyPaste.IsFloatingSelFromClipboard())
      {
        IndicateFileChange();
        SelectCopyPaste.KillFloatingSelection();
      }
      else
      {
        SelectCopyPaste.KillFloatingSelection();

        if (!Undo(&Level, TRUE))
          Gui.EnableUndo(FALSE);
      }

      Gui.EnableCopyDelete(FALSE);
      Gui.RedrawLevel();
      break;

    case ID_SELECTALL:
      {
        POINT Pt = {0, 0};

        ChangeEditMode(EM_SELECTDATA);
        SelectCopyPaste.Deselect(&Gui.GetScrollPos(), &Level);
        SelectCopyPaste.BeginFocusDragging(
          &Pt, &Gui.GetScrollPos(), Gui.GetDrawAreaWindow());

        Pt = Gui.GetScrollPage();
        Pt.x = (Pt.x - 1) * TILESIZE_EDITOR;
        Pt.y = (Pt.y - 1) * TILESIZE_EDITOR;

        SelectCopyPaste.FocusRectChange(&Pt);
        SelectCopyPaste.SelectionFinished(
          Gui.GetDrawAreaWindow(), &Gui.GetScrollPos(), &Level, NULL);
      }
      break;

    case ID_SHOWEDITOPS:
      ShowWindow(
        hWndEditOpsDlg,
        (Gui.CheckViewMenuItem(ID_SHOWEDITOPS)) ? SW_HIDE : SW_SHOW);
      break;

    case ID_SHOWACTORS:
      DrawActors = !DrawActors;
      Gui.CheckViewMenuItem(ID_SHOWACTORS);
      Gui.RedrawLevel();
      break;

    case ID_SHOWMASKED:
      DrawMasked = !DrawMasked;
      Gui.CheckViewMenuItem(ID_SHOWMASKED);
      Gui.RedrawLevel();
      break;

    case ID_SHOWGRID:
      DrawGrid = !DrawGrid;
      Gui.CheckViewMenuItem(ID_SHOWGRID);
      Gui.RedrawLevel();
      break;

    case ID_SHOWHOVERRECT:
      DrawHoverRectangle = !DrawHoverRectangle;
      Gui.CheckViewMenuItem(ID_SHOWHOVERRECT);
      Gui.RedrawLevel();
      break;

    case ID_LEVELINFO:
      DialogBoxParam(
        hInstance,
        MAKEINTRESOURCE(IDD_LEVELINFO),
        hWnd,
        DialogProc,
        IDD_LEVELINFO);
      break;

    case ID_PLACESOLID:
    case ID_PLACEMASKED:
    case ID_PLACEACTORS:
    case ID_SELECT:
      ChangeEditMode(EDITMODE(LOWORD(wParam) - 40041));
      break;

    case ID_SHOWMAPOVERVIEW:
      DialogBoxParam(
        hInstance,
        MAKEINTRESOURCE(IDD_LEVELOVERVIEW),
        hWnd,
        MapOverviewDlgProc,
        LPARAM(&Level));
      // ShowWindow(hWndLevelOverviewDlg,
      // (Gui.CheckViewMenuItem(ID_SHOWMAPOVERVIEW)) ? SW_HIDE : SW_SHOW);
      break;

    case ID_LEVELPROPS:
      LEVELPROPSPARAMS Params;
      Params.CZoneList = &CZoneList;
      Params.BackdropList = &BackdropList;
      Params.MusicList = &MusicList;
      Params.CreateNewLevel = FALSE;
      Params.Level = &Level;

      if (DialogBoxParam(
            hInstance,
            MAKEINTRESOURCE(IDD_LEVELPROPS),
            hWnd,
            LevelPropsDlgProc,
            LPARAM(&Params)))
      {
        ChangeCZone(Level.GetCZone());
        IndicateFileChange();
      }

      if (Params.LevelSizeChanged)
        Gui.LevelSizeChange(Level.GetLevelWidth(), Level.GetLevelHeight());
      break;

    case ID_NEWFILE:
      if (CheckIfFileWasSaved())
      {
        LEVELPROPSPARAMS Params;
        Params.CZoneList = &CZoneList;
        Params.BackdropList = &BackdropList;
        Params.MusicList = &MusicList;
        Params.CreateNewLevel = TRUE;
        Params.Level = &Level;

        if (DialogBoxParam(
              hInstance,
              MAKEINTRESOURCE(IDD_LEVELPROPS),
              hWnd,
              LevelPropsDlgProc,
              LPARAM(&Params)))
        {
          ChangeCZone(Level.GetCZone());
          NewFileOpened("");
        }
      }
      break;

    case ID_OPENFILE:
      of.lpstrFilter = FILENAME_FILTER;
      of.Flags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST |
        OFN_NOCHANGEDIR;
      of.lpstrFile = FileName;
      of.nMaxFile = 256;

      if (CheckIfFileWasSaved() && GetOpenFileName(&of))
      {
        SelectCopyPaste.Deselect(&Gui.GetScrollPos(), &Level);

        if (!LoadLevel(FileName, &Level))
        {
          Error("Failed to load level %s!", FileName);
          break;
        }

        ChangeCZone(Level.GetCZone());
        NewFileOpened(FileName);
      }
      break;

    case ID_SAVEFILE:
      if (CurrentFileName.size())
      {
        SaveLevel(CurrentFileName.c_str(), &Level);
        FileChanged = FALSE;
        break;
      }
      else
        ; // fallthrough

    case ID_SAVEFILEAS:
      of.lpstrFilter = FILENAME_FILTER;
      of.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
      of.lpstrFile = FileName;
      of.nMaxFile = 256;
      of.lpstrDefExt = ".mni";

      if (GetSaveFileName(&of))
      {
        SaveLevel(FileName, &Level);
        NewFileOpened(FileName, FALSE);
      }
      break;
  }
}

LRESULT OnCreate(HWND hWndCreate, WPARAM wParam, LPARAM lParam)
{
  // DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_CMPSPECIFY), hWnd,
  // DialogProc, IDD_CMPSPECIFY);

  // return -1;

  if (!CMP.Open(START_CMP))
  {
    Error("OnCreate(): Couldn't read CMP File %s!", START_CMP);
    return -1;
  }

  D3D.SetClearColor(D3DCOLOR(GetSysColor(COLOR_APPWORKSPACE)));
  return 0;
}

void OnPaint(HWND hWndPaint, WPARAM wParam, LPARAM lParam)
{
  PAINTSTRUCT ps;
  BeginPaint(hWndPaint, &ps);

  if (D3D_Ready)
  {
    POINT Point;
    GetCursorPos(&Point);
    ScreenToClient(Gui.GetDrawAreaWindow(), &Point);
    Point.x /= TILESIZE_EDITOR;
    Point.y /= TILESIZE_EDITOR;

    D3D.BeginSceneClear();
    DrawMap(&Gui.GetScrollPos(), &Gui.GetScrollPage(), &Point);

    if (SelectCopyPaste.IsThereFloatingSel())
      DrawFloatingSelection(&SelectCopyPaste, &Gui.GetScrollPos());

    if (
      DrawHoverRectangle && !ActorHovered && Gui.IsMouseInDrawArea() &&
      Gui.GetEditMode() < EM_PLACEACTORS)
    {
      Point.x = LastHoveredTile.x - Gui.GetScrollPos().x;
      Point.y = LastHoveredTile.y - Gui.GetScrollPos().y;
      DrawHoverRect(&Point);
    }

    if (D3DERR_DEVICELOST == D3D.EndScene())
    {
      D3D_Ready = FALSE;

      if (D3D.Reset(Gui.GetDrawAreaWindow(), TRUE, 0, 0))
      {
        ReloadGFX(&Gui.GetScrollPage());
        InvalidateRect(Gui.GetDrawAreaWindow(), NULL, FALSE);
        D3D_Ready = TRUE;
      }
    }
  }

  EndPaint(hWndPaint, &ps);
}

void OnTSPaint(HWND hWndTSPaint, WPARAM wParam, LPARAM lParam)
{
  PAINTSTRUCT ps;
  BeginPaint(hWndTSPaint, &ps);

  if (D3D_Ready)
  {
    D3D.BeginSceneClear();

    if (Gui.IsSolidTilesWin(hWndTSPaint))
      DrawSolidTiles(Gui.GetSTScrollPos(), Gui.GetSTScrollPage());
    else
      DrawMaskedTiles(GrayedOutLastMasked);

    D3D.EndScene(hWndTSPaint);
  }

  EndPaint(hWndTSPaint, &ps);
}

void OnSize(HWND hWndSize, WPARAM wParam, LPARAM lParam)
{
  Gui.OnSize(wParam, lParam);

  if (D3D_Ready && wParam != SIZE_MINIMIZED)
  {
    Gui.InitDAScrollbars(Level.GetLevelWidth(), Level.GetLevelHeight());

    D3D_Ready = FALSE;
    D3D.Reset(Gui.GetDrawAreaWindow(), TRUE, 0, 0);
    ReloadGFX(&Gui.GetScrollPage());
    D3D_Ready = TRUE;
    Gui.RedrawLevel();
  }
}

void OnLButtonDown(HWND hWndButton, WPARAM wParam, LPARAM lParam)
{
  if (SelectCopyPaste.IsDragging())
    return;

  POINT Pos = {
    LOWORD(lParam) / TILESIZE_EDITOR + Gui.GetScrollPos().x,
    HIWORD(lParam) / TILESIZE_EDITOR + Gui.GetScrollPos().y};

  if (
    Pos.x < 0 || Pos.y < 0 ||
    Pos.x >= Gui.GetScrollPage().x + Gui.GetScrollPos().x ||
    Pos.y >= Gui.GetScrollPage().y + Gui.GetScrollPos().y)
    return;

  LButtonPressed = TRUE;
  ActorChangePos = Pos;

  if (EM_SELECTDATA == Gui.GetEditMode())
  {
    POINT MousePos = {LOWORD(lParam), HIWORD(lParam)};
    SelectCopyPaste.BeginFocusDragging(
      &MousePos, &Gui.GetScrollPos(), Gui.GetDrawAreaWindow());
  }
}

void OnLButtonUp(HWND hWndButton, WPARAM wParam, LPARAM lParam)
{
  BOOL Pasted;

  if (SelectCopyPaste.SelectionFinished(
        Gui.GetDrawAreaWindow(), &Gui.GetScrollPos(), &Level, &Pasted))
  {
    Gui.EnableCopyDelete(SelectCopyPaste.IsSelectionActive());

    if (Pasted)
      IndicateFileChange();

    Gui.RedrawLevel();
    return;
  }

  if (!LButtonPressed)
    return;

  LButtonPressed = FALSE;

  if (SelectCopyPaste.IsSelectionActive())
    return;

  POINT Pos = {
    LOWORD(lParam) / TILESIZE_EDITOR + Gui.GetScrollPos().x,
    HIWORD(lParam) / TILESIZE_EDITOR + Gui.GetScrollPos().y};

  if (
    Pos.x < 0 || Pos.y < 0 ||
    Pos.x >= Gui.GetScrollPage().x + Gui.GetScrollPos().x ||
    Pos.y >= Gui.GetScrollPage().y + Gui.GetScrollPos().y)
    return;

  int y = 0;
  RECT r;

  LevelCell Cell;
  LevelCell *UndoCellsBefore, *UndoCellsAfter;
  Level.GetCellAttributes(&Pos, &Cell);

  switch (Gui.GetEditMode())
  {
    case EM_PLACESOLIDTILES:
      SetRect(
        &r, Pos.x, Pos.y, Pos.x + BrushSizeSolid.x, Pos.y + BrushSizeSolid.y);
      UndoCellsBefore = new LevelCell[(r.right - r.left) * (r.bottom - r.top)];
      UndoCellsAfter = new LevelCell[(r.right - r.left) * (r.bottom - r.top)];

      for (y = 0; y < BrushSizeSolid.y; y++, Pos.y++)
      {
        for (int x = 0; x < BrushSizeSolid.x; x++, Pos.x++)
        {
          Level.GetCellAttributes(&Pos, &Cell);

          if (
            CurrentSolidTile + y * 40 + x < 1000 &&
            (CurrentSolidTile % 40 + x) < 40 &&
            !(Cell.bMasked && Cell.Masked >= 128))
          {
            memcpy(
              &UndoCellsBefore[x + y * (r.right - r.left)],
              &Cell,
              sizeof(LevelCell));
            Level.SetCellAttributes(
              &Pos,
              Cell.Solid = CurrentSolidTile + y * 40 + x,
              (Cell.bMasked) ? Cell.Masked : 0,
              Cell.bMasked);
            memcpy(
              &UndoCellsAfter[x + y * (r.right - r.left)],
              &Cell,
              sizeof(LevelCell));
          }
        }

        Pos.x -= BrushSizeSolid.x;
      }

      AddUndoState(&r, UndoCellsBefore);
      AddUndoState(&r, UndoCellsAfter);
      delete UndoCellsBefore;
      delete UndoCellsAfter;
      break;

    case EM_PLACEMASKEDTILES:
      SetRect(
        &r, Pos.x, Pos.y, Pos.x + BrushSizeMasked.x, Pos.y + BrushSizeMasked.y);
      UndoCellsBefore = new LevelCell[(r.right - r.left) * (r.bottom - r.top)];
      UndoCellsAfter = new LevelCell[(r.right - r.left) * (r.bottom - r.top)];

      for (y = 0; y < BrushSizeMasked.y; y++, Pos.y++)
      {
        for (int x = 0; x < BrushSizeMasked.x; x++, Pos.x++)
        {
          Level.GetCellAttributes(&Pos, &Cell);

          if (
            CurrentMaskedTile + y * 40 + x < 160 &&
            (CurrentMaskedTile % 40 + x) < 40 &&
            !(CurrentMaskedTile + y * 40 + x >= 128 && Cell.Solid))
          {
            memcpy(
              &UndoCellsBefore[x + y * (r.right - r.left)],
              &Cell,
              sizeof(LevelCell));
            Level.SetCellAttributes(
              &Pos,
              Cell.Solid,
              Cell.Masked =
                ((wParam & MK_SHIFT) ? 0 : CurrentMaskedTile + y * 40 + x),
              Cell.bMasked = ((wParam & MK_SHIFT) ? FALSE : TRUE));
            memcpy(
              &UndoCellsAfter[x + y * (r.right - r.left)],
              &Cell,
              sizeof(LevelCell));
          }
        }

        Pos.x -= BrushSizeMasked.x;
      }

      AddUndoState(&r, UndoCellsBefore);
      AddUndoState(&r, UndoCellsAfter);
      delete UndoCellsBefore;
      delete UndoCellsAfter;
      break;

    case EM_PLACEACTORS:
      SetRect(&r, Pos.x, Pos.y, Pos.x + 1, Pos.y + 1);
      Level.GetCellAttributes(&Pos, &Cell);
      AddUndoState(&r, &Cell);

      if (MK_SHIFT & wParam)
        Level.DeleteActor(&Pos);
      else
        Level.SetActor(&Pos, CurrentActor);

      Level.GetCellAttributes(&Pos, &Cell);
      AddUndoState(&r, &Cell);
      break;
  }

  IndicateFileChange();
  Gui.RedrawLevel();
}

BOOL OnContextMenu(HWND hWndContextMenu, WPARAM wParam, LPARAM lParam)
{
  POINT Pos = {LOWORD(lParam), HIWORD(lParam)};

  ScreenToClient(Gui.GetDrawAreaWindow(), &Pos);

  Pos.x = Pos.x / TILESIZE_EDITOR + Gui.GetScrollPos().x;
  Pos.y = Pos.y / TILESIZE_EDITOR + Gui.GetScrollPos().y;

  if (
    Pos.x < 0 || Pos.y < 0 || Pos.x >= Level.GetLevelWidth() ||
    Pos.y >= Level.GetLevelHeight())
    return FALSE;

  LevelCell Cell;
  Level.GetCellAttributes(&Pos, &Cell);

  ActorChangePos.x = Pos.x;
  ActorChangePos.y = Pos.y;

  Gui.OnContextMenu(hWndContextMenu, wParam, lParam, Cell.Actor);

  return TRUE;
}

void OnMouseMove(HWND hWndMM, WPARAM wParam, LPARAM lParam)
{
  POINT MousePos = {LOWORD(lParam), HIWORD(lParam)};

  if (wParam & MK_LBUTTON)
    if (SelectCopyPaste.FocusRectChange(&MousePos))
      return;

  POINT Pos = {
    MousePos.x / TILESIZE_EDITOR + Gui.GetScrollPos().x,
    MousePos.y / TILESIZE_EDITOR + Gui.GetScrollPos().y};

  if (
    Pos.x >= 0 && Pos.y >= 0 &&
    Pos.x < Gui.GetScrollPage().x + Gui.GetScrollPos().x &&
    Pos.y < Gui.GetScrollPage().y + Gui.GetScrollPos().y &&
    Pos.x < Level.GetLevelWidth() && Pos.y < Level.GetLevelHeight())
  {
    LevelCell Cell;
    Level.GetCellAttributes(&Pos, &Cell);
    char StatusText[256];

    sprintf(StatusText, "%d, %d", Pos.x, Pos.y);
    Gui.SetStatusText(1, StatusText);
    sprintf(StatusText, "%d", Cell.Solid);
    Gui.SetStatusText(2, StatusText);

    if (Cell.bMasked && DrawMasked)
    {
      sprintf(StatusText, "%d", Cell.Masked);
      Gui.SetStatusText(3, StatusText);
    }
    else
      Gui.SetStatusText(3, NULL);

    if (Cell.Actor && DrawActors)
    {
      WORD ActorID = 0;
      Level.GetActor(&Pos, &ActorID);
      sprintf(StatusText, "%u - %s", ActorID, ActorDefs[ActorID].Name);
      Gui.SetStatusText(4, StatusText);
      ActorHovered = TRUE;
    }
    else if (ActorHovered)
    {
      ActorHovered = FALSE;
      Gui.SetStatusText(4, NULL);
    }

    if ((Pos.x != LastHoveredTile.x || Pos.y != LastHoveredTile.y))
    {
      SelectCopyPaste.MoveSelection(&Pos, &LastHoveredTile);
      Gui.SetMoveCursor(
        SelectCopyPaste.PointInSelection(&MousePos, &Gui.GetScrollPos()));
      LastHoveredTile = Pos;
      Gui.MouseEnteredDrawArea();
      Gui.RedrawLevel();

      LevelCell Cell;
      Level.GetCellAttributes(&LastHoveredTile, &Cell);

      if (GrayedOutLastMasked != Cell.Solid > 0)
      {
        GrayedOutLastMasked = !GrayedOutLastMasked;
        Gui.RedrawMaskedTiles();
      }
    }
  }
  else
    Gui.MouseLeftDrawArea();
}

void InitMessages(void)
{
  CreateNameLists();

  D3D_Ready = TRUE;

  hPen = CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
  ChangeEditMode(EM_PLACESOLIDTILES);

  hWndEditOpsDlg = CreateDialogParam(
    hInstance, MAKEINTRESOURCE(IDD_EDITOPS), hWnd, DialogProc, IDD_EDITOPS);

  RECT r, r2;
  GetClientRect(hWnd, &r);
  GetWindowRect(hWndEditOpsDlg, &r2);
  SetWindowPos(
    hWndEditOpsDlg,
    NULL,
    r.right - (r2.right - r2.left) - 20,
    40,
    0,
    0,
    SWP_NOSIZE | SWP_NOZORDER);
}

void CleanUpMessages(void)
{
  DeleteObject(hPen);
  DeleteObject(hBmActorPreview);
  CleanUpUndoRedo();
}