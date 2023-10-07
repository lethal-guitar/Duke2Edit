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


#ifndef MAP_H
#define MAP_H

#include "Duke2Files.h"
#include <windows.h>

struct LevelCell
{
  WORD Solid;
  BYTE Masked;
  BOOL Actor;
  WORD ActorID;
  BOOL bMasked;
};

enum LEVELFLAGS
{
  LF_SCROLLHORZVERT = 0x01,
  LF_SCROLLHORZ = 0x02,
  LF_UNKNOWN = 0x04,
  LF_AUTOSCROLLHORZ = 0x08,
  LF_AUTOSCROLLVERT = 0x10,
  LF_EARTHQUAKE = 0x20,
  LF_CHANGEBG1 = 0x40,
  LF_CHANGEBG2 = 0x80
};

const int VALID_LEVEL_WIDTHS[] = {64, 128, 256, 512, 1024, 0};

class Duke2Map
{
private:
  int LevelWidth;
  LevelCell*** LevelData;

  char CZone[13];
  char Music[13];
  char BGDrop[13];
  WORD Flags;
  int NrActors;

public:
  Duke2Map(void) { ZeroMemory(this, sizeof(Duke2Map)); }
  ~Duke2Map() { CleanUp(); }

  friend void DrawMap(POINT*, POINT*, POINT*);
  friend HBITMAP CreateLevelOverview(Duke2Map* Level);

  void CleanUp(void);
  void Create(int Width, char* CZone, char* Music, char* BGDrop, WORD Flags);

  int GetLevelWidth(void) { return LevelWidth; }
  int GetLevelHeight(void) { return LEVEL_DATA_WORDS / LevelWidth; }
  char* GetCZone(void) { return CZone; }
  char* GetMusic(void) { return Music; }
  char* GetBGDrop(void) { return BGDrop; }
  WORD GetFlags(void) { return Flags; }
  int GetNrActors(void) { return NrActors; }
  int GetNrSolidTiles(void);
  int GetNrMaskedTiles(void);

  void SetCZone(const char* CZone) { strcpy(this->CZone, CZone); }
  void SetBGDrop(const char* BGDrop) { strcpy(this->BGDrop, BGDrop); }
  void SetMusic(const char* Music) { strcpy(this->Music, Music); }
  void SetFlags(WORD Flags) { this->Flags = Flags; }
  void ChangeWidth(WORD NewWidth);

  void SetCellAttributes(POINT* Pos, WORD Solid, BYTE Masked, BOOL bMasked);
  BOOL GetCellAttributes(POINT* Pos, LevelCell* Cell);

  void SetActor(POINT* Pos, WORD ID)
  {
    LevelData[Pos->y][Pos->x]->ActorID = ID;

    if (!LevelData[Pos->y][Pos->x]->Actor)
    {
      NrActors++;
      LevelData[Pos->y][Pos->x]->Actor = TRUE;
    }
  }

  void DeleteActor(POINT* Pos);

  BOOL GetActor(POINT* Pos, WORD* pActorID);
};

#endif