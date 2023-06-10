#ifndef SELECTCOPYPASTE_H
#define SELECTCOPYPASTE_H

#include <windows.h>
#include "Map.h"
#include "Constants.h"

const int CLIPBOARDFORMAT_ID = CF_PRIVATEFIRST+1;

extern class SelectCopyPasteClass
{
	private:
		HGLOBAL    hClipboardData;
		HDC        hDCFocus;
		POINT      FocusOrigin, StartScrollPos;
		BOOL       Dragging, SelectionActive, MovingSelection, FloatingSelIsFromClipboard;
		RECT       FocusRect, FloatingSelDst, FloatingSelOrigin;
		LevelCell* FloatingSelection, *DelUndoBefore, *DelUndoAfter;

	public:
		~SelectCopyPasteClass() { delete FloatingSelection; delete DelUndoBefore; delete DelUndoAfter; }
		
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
		
		void OnDestroyClipboard(void) { GlobalFree(hClipboardData); hClipboardData = NULL; }
		
		void BeginFocusDragging(POINT* Pos, POINT* ScrollPos, HWND hWndFD);
		void EndFocusDragging(HWND hWndFD)
		{
			Dragging = FALSE;
			DrawFocusRect(hDCFocus, &FocusRect);
			ReleaseDC(hWndFD, hDCFocus);
		}

		void MoveSelection(POINT* Pos, POINT* LastHoveredTile) { 
			if (MovingSelection) 
				OffsetRect(&FocusRect, (Pos->x - LastHoveredTile->x)*TILESIZE_EDITOR, (Pos->y - LastHoveredTile->y)*TILESIZE_EDITOR);
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
		BOOL PointInSelection(POINT* pPos, POINT* ScrollPos) { POINT Pos = { pPos->x + ScrollPos->x*TILESIZE_EDITOR, pPos->y + ScrollPos->y*TILESIZE_EDITOR }; return SelectionActive && PtInRect(&FloatingSelDst, Pos) || MovingSelection; }
		void GetFloatingSelRect(RECT* r) { *r = FocusRect; }
} SelectCopyPaste;

#endif