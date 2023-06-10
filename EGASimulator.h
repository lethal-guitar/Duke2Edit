#ifndef EGASIMULATOR_H
#define EGASIMULATOR_H

#include <windows.h>
#include "Shared.h"

struct PALENTRY
{
	BYTE r, g, b;
};

class EGASimulator
{
	private:
		BYTE*	  Memory;
		BYTE*     Mask;
		BYTE	  Plane;
		PALENTRY  Palette[16];
		BOOL	  WriteInProgress;
		POINT     ImageSize;
		//int       Position;

		void	  Convert6bitPalette(void);

	public:
		EGASimulator(void) { Mask = new BYTE; Memory = new BYTE; ZeroMemory(this, sizeof(EGASimulator)); }
		~EGASimulator()	   { delete[] Mask; delete[] Memory; }

		void LoadPalette(PALENTRY* pPal);
		void LoadPaletteFromFile(char* FileName);
		void GetPalette(PALENTRY* pPalOut) { memcpy(pPalOut, Palette, sizeof(PALENTRY)*16); }
		
		//void BeginWrite(void) { ZeroMemory(Memory, sizeof(BYTE)*64000); WriteInProgress = TRUE; }
		void BeginWrite(int dx, int dy);
		void EndWrite(void) { WriteInProgress = FALSE; }
		void SelectPlane(BYTE Pl) { if (Pl > 3) { Error("EGASimulator::SelectPlane(): Invalid plane number (too high)!"); return; } Plane = Pl; }
		void WriteByte(int Position, BYTE Value);
		void WriteMaskByte(int Position, BYTE Value);
		
		void TransferImage(DWORD* pRawDataOut, RECT* r = NULL);
		void TransferImage(HBITMAP* phBmOut, HDC hDC, RECT* r = NULL);
		void TransferMask(HBITMAP* phBmOut, HDC hDC, RECT* r = NULL);
};



#endif