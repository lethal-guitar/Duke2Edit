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


#include "EGASimulator.h"

void EGASimulator::Convert6bitPalette(void)
{
  for (int i = 0; i < 16; i++)
  {
    WORD Tmp;

    Tmp = Palette[i].r << 2;
    if (Tmp)
      Tmp--;
    Palette[i].r = BYTE(Tmp);

    Tmp = Palette[i].g << 2;
    if (Tmp)
      Tmp--;
    Palette[i].g = BYTE(Tmp);

    Tmp = Palette[i].b << 2;
    if (Tmp)
      Tmp--;
    Palette[i].b = BYTE(Tmp);
  }
}

void EGASimulator::LoadPalette(PALENTRY* pPal)
{
  if (WriteInProgress)
  {
    Error(
      "EGASimulator::LoadPalette(): Write is in progress, can't change palette now!");
    return;
  }

  try
  {
    memcpy(Palette, pPal, sizeof(PALENTRY) * 16);
  }
  catch (...)
  {
    Error(
      "EGASimulator::LoadPalette(): Exception raised during palette copy, copy failed! Original palette restored!");
    return;
  }

  Convert6bitPalette();
}

void EGASimulator::LoadPaletteFromFile(char* FileName)
{
  if (WriteInProgress)
  {
    Error(
      "EGASimulator::LoadPaletteFromFile(): Write is in progress, can't change palette now!");
    return;
  }

  HANDLE hFile =
    CreateFile(FileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);

  if (!hFile)
  {
    Error("EGASimulator::LoadPaletteFromFile(): Failed to open source file!");
    return;
  }

  if (GetFileSize(hFile, NULL) != 48)
  {
    Error("EGASimulator::LoadPaletteFromFile(): Source file has wrong size!");
    CloseHandle(hFile);
    return;
  }

  PALENTRY Pal[16];
  DWORD NrRead;

  if (
    !ReadFile(hFile, Pal, sizeof(PALENTRY) * 16, &NrRead, NULL) ||
    NrRead != sizeof(PALENTRY) * 16)
  {
    Error(
      "EGASimulator::LoadPaletteFromFile(): Reading from source file failed!");
    CloseHandle(hFile);
    return;
  }

  CloseHandle(hFile);

  memcpy(Palette, Pal, sizeof(PALENTRY) * 16);
  Convert6bitPalette();
}

void EGASimulator::BeginWrite(int dx, int dy)
{
  ImageSize.x = dx;
  ImageSize.y = dy;

  delete Mask;
  delete Memory;

  Memory = new BYTE[ImageSize.x * ImageSize.y];
  Mask = new BYTE[ImageSize.x * ImageSize.y];

  ZeroMemory(Memory, sizeof(BYTE) * ImageSize.x * ImageSize.y);
  memset(Mask, 1, sizeof(BYTE) * ImageSize.x * ImageSize.y);

  WriteInProgress = TRUE;
}

void EGASimulator::WriteByte(int Position, BYTE Value)
{
  if (!WriteInProgress)
  {
    Error(
      "EGASimulator::WriteByte(): BeginWrite() must be called before writing bytes!");
    return;
  }

  for (int Bit = 0; Bit < 8; Bit++)
  {
    Memory[Position * 8 + Bit] += ((Value & 0x80) >> 7) << Plane;
    Value <<= 1;
  }
}

void EGASimulator::WriteMaskByte(int Position, BYTE Value)
{
  if (!WriteInProgress)
  {
    Error(
      "EGASimulator::WriteByte(): BeginWrite() must be called before writing bytes!");
    return;
  }

  for (int Bit = 0; Bit < 8; Bit++)
  {
    Mask[Position * 8 + Bit] = !((Value & 0x80) >> 7);
    Value <<= 1;
  }
}

void EGASimulator::TransferImage(DWORD* pRawDataOut, RECT* r)
{
  if (WriteInProgress)
  {
    Error(
      "EGASimulator::TransferImage(): Write is in progress, can't transfer image now!");
    return;
  }

  for (int y = 0; y < ImageSize.y; y++)
    for (int x = 0; x < ImageSize.x; x++)
      pRawDataOut[y * ImageSize.x + x] =
        (((Mask[y * ImageSize.x + x]) ? 0xFF : 0x00) << 24) +
        (Palette[Memory[y * ImageSize.x + x]].r << 16) +
        (Palette[Memory[y * ImageSize.x + x]].g << 8) +
        (Palette[Memory[y * ImageSize.x + x]].b);
}

void EGASimulator::TransferImage(HBITMAP* phBmOut, HDC hDC, RECT* pr)
{
  if (WriteInProgress)
  {
    Error(
      "EGASimulator::TransferImage(): Write is in progress, can't transfer image now!");
    return;
  }

  RECT r;

  if (!pr)
    SetRect(&r, 0, 0, ImageSize.x, ImageSize.y);
  else
    SetRect(&r, pr->left, pr->top, pr->right, pr->bottom);

  struct
  {
    BITMAPINFOHEADER bmih;
    RGBQUAD bmiColors[16];
  } bmi;

  ZeroMemory(&bmi, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 16);

  bmi.bmih.biSize = sizeof(BITMAPINFOHEADER);
  bmi.bmih.biWidth = r.right - r.left;
  bmi.bmih.biHeight = r.bottom - r.top;
  bmi.bmih.biPlanes = 1;
  bmi.bmih.biBitCount = 4;
  bmi.bmih.biCompression = BI_RGB;

  for (int i = 0; i < 16; i++)
  {
    bmi.bmiColors[i].rgbRed = Palette[i].r;
    bmi.bmiColors[i].rgbGreen = Palette[i].g;
    bmi.bmiColors[i].rgbBlue = Palette[i].b;
  }

  BYTE* Data = new BYTE[(bmi.bmih.biWidth * bmi.bmih.biHeight) / 2];

  int NrByte = 0;

  for (int y = r.bottom - 1; y >= r.top; y--)
  {
    for (int x = r.left; x < r.right; x++)
    {
      if (x % 2 == 0)
      {
        Data[NrByte] = Memory[y * ImageSize.x + x] << 4;
      }
      else
      {
        Data[NrByte] += Memory[y * ImageSize.x + x];
        NrByte++;
      }
    }
  }

  *phBmOut = CreateDIBitmap(
    hDC, &bmi.bmih, CBM_INIT, Data, LPBITMAPINFO(&bmi), DIB_RGB_COLORS);

  if (!*phBmOut)
    Error("EGASimulator::TransferImage(): CreateDIBitmap() failed!");

  delete[] Data;
}

void EGASimulator::TransferMask(HBITMAP* phBmOut, HDC hDC, RECT* pr)
{
  if (WriteInProgress)
  {
    Error(
      "EGASimulator::TransferImage(): Write is in progress, can't transfer image now!");
    return;
  }

  RECT r;

  if (!pr)
    SetRect(&r, 0, 0, ImageSize.x, ImageSize.y);
  else
    SetRect(&r, pr->left, pr->top, pr->right, pr->bottom);

  struct
  {
    BITMAPINFOHEADER bmih;
    RGBQUAD bmiColors[2];
  } bmi;

  ZeroMemory(&bmi, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 2);

  bmi.bmih.biSize = sizeof(BITMAPINFOHEADER);
  bmi.bmih.biWidth = r.right - r.left;
  bmi.bmih.biHeight = r.bottom - r.top;
  bmi.bmih.biPlanes = 1;
  bmi.bmih.biBitCount = 1;
  bmi.bmih.biCompression = BI_RGB;

  bmi.bmiColors[0].rgbRed = 0;
  bmi.bmiColors[0].rgbGreen = 0;
  bmi.bmiColors[0].rgbBlue = 0;
  bmi.bmiColors[1].rgbRed = 0xFF;
  bmi.bmiColors[1].rgbGreen = 0xFF;
  bmi.bmiColors[1].rgbBlue = 0xFF;

  /*
  for (int i=0; i<2; i++)
  {
          bmi.bmiColors[i].rgbRed = Palette[i].r;
          bmi.bmiColors[i].rgbGreen = Palette[i].g;
          bmi.bmiColors[i].rgbBlue = Palette[i].b;
  }*/

  /*
  BYTE* Data = new BYTE[(bmi.bmih.biWidth*bmi.bmih.biHeight)/2];

  int NrByte = 0;

  for (int y=r.bottom-1; y>=r.top; y--)
  {
          for (int x=r.left; x<r.right; x++)
          {
                  if (x % 2 == 0)
                  {
                          if (Mask[y*ImageSize.x+x])
                                  Data[NrByte] = 0xF0;
                          else
                                  Data[NrByte] = 0x0 << 4;
                  }
                  else
                  {
                          if (Mask[y*ImageSize.x+x])
                                  Data[NrByte] += 0x0F;
                          else
                                  Data[NrByte] += 0x0;

                          NrByte++;
                  }
          }
  }*/

  int Off = (ImageSize.x / 8) % 4; //(4 - (ImageSize.x/8)%4)*8;

  if (Off)
    Off = 4 - Off;

  Off *= 8;

  BYTE* Data = new BYTE[(ImageSize.x + Off) * ImageSize.y / 8];
  ZeroMemory(Data, (ImageSize.x + Off) * ImageSize.y / 8);

  int di = 0, bi = 0;

  for (int y = r.bottom - 1; y >= r.top; y--)
  {
    for (int x = r.left; x < r.right; x++)
    {
      Data[di] += Mask[y * ImageSize.x + x] << (7 - (bi));

      if (8 == ++bi)
      {
        di++;
        bi = 0;
      }
    }

    di += Off / 8;
  }

  *phBmOut = CreateDIBitmap(
    hDC, &bmi.bmih, CBM_INIT, Data, LPBITMAPINFO(&bmi), DIB_RGB_COLORS);

  if (!*phBmOut)
    Error("EGASimulator::TransferImage(): CreateDIBitmap() failed!");

  delete[] Data;
}