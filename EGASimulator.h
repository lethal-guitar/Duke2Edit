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


#ifndef EGASIMULATOR_H
#define EGASIMULATOR_H

#include "Shared.h"
#include <windows.h>

struct PALENTRY
{
  BYTE r, g, b;
};

class EGASimulator
{
private:
  BYTE* Memory;
  BYTE* Mask;
  BYTE Plane;
  PALENTRY Palette[16];
  BOOL WriteInProgress;
  POINT ImageSize;
  // int       Position;

  void Convert6bitPalette(void);

public:
  EGASimulator(void)
  {
    Mask = new BYTE;
    Memory = new BYTE;
    ZeroMemory(this, sizeof(EGASimulator));
  }
  ~EGASimulator()
  {
    delete[] Mask;
    delete[] Memory;
  }

  void LoadPalette(PALENTRY* pPal);
  void LoadPaletteFromFile(char* FileName);
  void GetPalette(PALENTRY* pPalOut)
  {
    memcpy(pPalOut, Palette, sizeof(PALENTRY) * 16);
  }

  // void BeginWrite(void) { ZeroMemory(Memory, sizeof(BYTE)*64000);
  // WriteInProgress = TRUE; }
  void BeginWrite(int dx, int dy);
  void EndWrite(void) { WriteInProgress = FALSE; }
  void SelectPlane(BYTE Pl)
  {
    if (Pl > 3)
    {
      Error("EGASimulator::SelectPlane(): Invalid plane number (too high)!");
      return;
    }
    Plane = Pl;
  }
  void WriteByte(int Position, BYTE Value);
  void WriteMaskByte(int Position, BYTE Value);

  void TransferImage(DWORD* pRawDataOut, RECT* r = NULL);
  void TransferImage(HBITMAP* phBmOut, HDC hDC, RECT* r = NULL);
  void TransferMask(HBITMAP* phBmOut, HDC hDC, RECT* r = NULL);
};


#endif