#ifndef GUI_H
#define GUI_H

#include <windows.h>
#include <commctrl.h>
#include "Map.h"
#include "Shared.h"
#include "Constants.h"
#include "resource.h"

enum EDITMODE
{
	EM_PLACESOLIDTILES,
	EM_PLACEMASKEDTILES,
	EM_PLACEACTORS,
	EM_SELECTDATA
};

const char DRAWAREA_WNDCLASSNAME[] = "D2EDAClass";
const char SOLIDTILES_WNDCLASSNAME[] = "D2ESTClass";
const char MASKEDTILES_WNDCLASSNAME[] = "D2EMTClass";
const int  STATUSBAR_ID	  = 1024;
const int  TOOLBAR_ID     = 2048;
const int  NR_TOOLBAR_BTNS = 17;
const int  NR_STATUSBAR_PARTS = 5;

extern class GuiClass
{
	private:
		HWND  hWndStatusBar, hWndDrawArea, hWndSolidTiles, 
			  hWndMaskedTiles, hWndToolbar;
		HMENU hMenuPopup, hMenuP, hMenuView;
		HCURSOR hCursorDrawTile, hCursorStd, hCursorMove, hDACursor;
		int   StatusBar_Height, MaskedTiles_Height, 
			  SolidTiles_Height, DrawArea_HeightOffset, Toolbar_Height;
		EDITMODE EditMode;
		long  STPos, STPage;
		SCROLLINFO ScrollInfo;
		POINT ScrollPos, Page;
		RECT  DAClientRect;
		BOOL  MouseInDrawArea;

		void RedrawWindow(HWND hWindow)
		{
			InvalidateRect(hWindow, NULL, FALSE);
			UpdateWindow(hWindow);
		}

		void InitScrollBars(HWND hWndSBar, UINT PageHorz, UINT PageVert, int MaxHorz, int MaxVert);

	public:
		~GuiClass() 
		{
			DestroyMenu(hMenuP);
			DestroyCursor(hCursorDrawTile);
			DestroyCursor(hCursorStd);
			DestroyCursor(hCursorMove);
		}

		BOOL Init(void);
		void SetEditMode(EDITMODE EditMode);
		void OnSTVertScroll(HWND hWndSBar, WPARAM wParam, LPARAM lParam);
		void OnVertScroll(HWND hWndSBar, WPARAM wParam, LPARAM lParam);
		void OnHorizScroll(HWND hWndSBar, WPARAM wParam, LPARAM lParam);
		void SizeStatusbarParts(void);

		void EnableCopyDelete(BOOL Enable)
		{
			EnableMenuItem(GetSubMenu(GetMenu(hWnd), 1), ID_CUT, MF_BYCOMMAND | ((Enable) ? MF_ENABLED : MF_GRAYED));
			EnableMenuItem(GetSubMenu(GetMenu(hWnd), 1), ID_COPY, MF_BYCOMMAND | ((Enable) ? MF_ENABLED : MF_GRAYED));
			EnableMenuItem(GetSubMenu(GetMenu(hWnd), 1), ID_DELETE, MF_BYCOMMAND | ((Enable) ? MF_ENABLED : MF_GRAYED));
			EnableMenuItem(hMenuPopup, ID_CUT, MF_BYCOMMAND | ((Enable) ? MF_ENABLED : MF_GRAYED));
			EnableMenuItem(hMenuPopup, ID_COPY, MF_BYCOMMAND | ((Enable) ? MF_ENABLED : MF_GRAYED));
			EnableMenuItem(hMenuPopup, ID_DELETE, MF_BYCOMMAND | ((Enable) ? MF_ENABLED : MF_GRAYED));
			SendMessage(hWndToolbar, TB_ENABLEBUTTON, ID_CUT, MAKELONG(Enable, 0));
			SendMessage(hWndToolbar, TB_ENABLEBUTTON, ID_COPY, MAKELONG(Enable, 0));
		}
		
		void EnableUndo(BOOL Enable)
		{
			SendMessage(hWndToolbar, TB_ENABLEBUTTON, ID_UNDO, MAKELONG(Enable, 0));
			EnableMenuItem(GetSubMenu(GetMenu(hWnd), 1), ID_UNDO, MF_BYCOMMAND | ((Enable) ? MF_ENABLED : MF_GRAYED));
		}
		
		void EnableRedo(BOOL Enable)
		{
			SendMessage(hWndToolbar, TB_ENABLEBUTTON, ID_REDO, MAKELONG(Enable, 0));
			EnableMenuItem(GetSubMenu(GetMenu(hWnd), 1), ID_REDO, MF_BYCOMMAND | ((Enable) ? MF_ENABLED : MF_GRAYED));
		}
		
		void MouseLeftDrawArea(void)
		{
			if (MouseInDrawArea)
			{
				SendMessage(hWndStatusBar, SB_SETTEXT, 1, NULL);
				SendMessage(hWndStatusBar, SB_SETTEXT, 2, NULL);
				SendMessage(hWndStatusBar, SB_SETTEXT, 3, NULL);
				SendMessage(hWndStatusBar, SB_SETTEXT, 4, NULL);
				SetCursor(hCursorStd);
				MouseInDrawArea = FALSE;
				RedrawLevel();
			}
		}
		
		void MouseEnteredDrawArea(void) { MouseInDrawArea = TRUE; };


		void RedrawLevel(void) { RedrawWindow(hWndDrawArea); }
		void RedrawMaskedTiles(void) { RedrawWindow(hWndMaskedTiles); }
		
		void RedrawCZone(void)
		{
			RedrawWindow(hWndSolidTiles);
			RedrawWindow(hWndMaskedTiles);
		}

		void LevelSizeChange(WORD LevelWidth, WORD LevelHeight)
		{
			InitScrollBars(hWndDrawArea, Page.x, Page.y, LevelWidth-1, LevelHeight-1);
			RedrawLevel();
		}

		void EnablePaste(BOOL Enable)
		{
			EnableMenuItem(GetSubMenu(GetMenu(hWnd), 1), ID_PASTE, MF_BYCOMMAND | ((Enable) ? MF_ENABLED : MF_GRAYED));
			EnableMenuItem(hMenuPopup, ID_PASTE, MF_BYCOMMAND | ((Enable) ? MF_ENABLED : MF_GRAYED));
			SendMessage(hWndToolbar, TB_ENABLEBUTTON, ID_PASTE, MAKELONG(Enable, 0));
		}

		void OnSize(WPARAM wParam, LPARAM lParam)
		{
			SendMessage(hWndToolbar, TB_AUTOSIZE, 0, 0);
			SendMessage(hWndStatusBar, WM_SIZE, wParam, lParam);
			SizeStatusbarParts();
			MoveWindow(hWndDrawArea, 0, Toolbar_Height, LOWORD(lParam), HIWORD(lParam) - DrawArea_HeightOffset, TRUE);
			SetWindowPos(hWndSolidTiles, NULL, 0, HIWORD(lParam) - SolidTiles_Height - MaskedTiles_Height - StatusBar_Height, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			SetWindowPos(hWndMaskedTiles, NULL, 0, HIWORD(lParam) - MaskedTiles_Height - StatusBar_Height + 1, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		}

		void SetMainWindowCaption(const char* Cap) { SetWindowText(hWnd, Cap); }

		void InitCZoneScrollbar(void) { InitScrollBars(hWndSolidTiles, 0, STPage = 128/TILESIZE_EDITOR, 0, 400/TILESIZE_EDITOR - 1); }

		void InitDAScrollbars(WORD LevelWidth, WORD LevelHeight)
		{
			GetClientRect(hWndDrawArea, &DAClientRect);
			InitScrollBars(hWndDrawArea, Page.x = DAClientRect.right / TILESIZE_EDITOR, Page.y = DAClientRect.bottom / TILESIZE_EDITOR, LevelWidth-1, LevelHeight-1);
		}

		void SetStatusText(int p, char* Text) { SendMessage(hWndStatusBar, SB_SETTEXT, p, LPARAM(Text)); }
		BOOL IsSolidTilesWin(HWND hWndQ) { return hWndSolidTiles == hWndQ; }
		long GetSTScrollPos(void) { return STPos; }
		long GetSTScrollPage(void) { return STPage; }
		POINT GetScrollPos(void) { return ScrollPos; }
		POINT GetScrollPage(void) { return Page; }
		HWND GetDrawAreaWindow(void) { return hWndDrawArea; }
		EDITMODE GetEditMode(void) { return EditMode; }
		
		void OnContextMenu(HWND hWndContextMenu, WPARAM wParam, LPARAM lParam, BOOL Actor)
		{
			EnableMenuItem(hMenuPopup, ID_DELETEACTOR, MF_BYCOMMAND | ((Actor) ? MF_ENABLED : MF_GRAYED));
			TrackPopupMenuEx(hMenuPopup, TPM_LEFTALIGN | TPM_LEFTBUTTON, LOWORD(lParam), HIWORD(lParam), hWndContextMenu, NULL);
		}

		BOOL CheckViewMenuItem(UINT Id)
		{
			UINT Mask = GetMenuState(hMenuView, Id, MF_BYCOMMAND);	
			CheckMenuItem(hMenuView, Id, ((Mask & MF_CHECKED) ? MF_UNCHECKED : MF_CHECKED) | MF_BYCOMMAND);

			return Mask & MF_CHECKED;
		}
		
		BOOL IsMouseInDrawArea(void) { return MouseInDrawArea; }

		/*
		BOOL OnShowEditOps(void)
		{
			UINT Mask = GetMenuState(hMenuView, ID_SHOWEDITOPS, MF_BYCOMMAND);	
			CheckMenuItem(hMenuView, ID_SHOWEDITOPS, ((Mask & MF_CHECKED) ? MF_UNCHECKED : MF_CHECKED) | MF_BYCOMMAND);

			return Mask & MF_CHECKED;
		}

		void OnShowActors(void)
		{
			UINT Mask = GetMenuState(hMenuView, ID_SHOWACTORS, MF_BYCOMMAND);
			CheckMenuItem(hMenuView, ID_SHOWACTORS, ((Mask & MF_CHECKED) ? MF_UNCHECKED : MF_CHECKED) | MF_BYCOMMAND);
		}

		void OnShowMaskedTiles(void)
		{
			UINT Mask = GetMenuState(hMenuView, ID_SHOWMASKED, MF_BYCOMMAND);
			CheckMenuItem(hMenuView, ID_SHOWMASKED, ((Mask & MF_CHECKED) ? MF_UNCHECKED : MF_CHECKED) | MF_BYCOMMAND);
		}*/

		void SetMoveCursor(BOOL MoveCursor) { SetCursor((MoveCursor) ? hCursorMove : hDACursor); }
} Gui;

#endif