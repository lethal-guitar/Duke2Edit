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


#ifndef EDITORGFX_H
#define EDITORGFX_H

#include "CMP.h"
#include "Constants.h"
#include "Map.h"
#include "SelectCopyPaste.h"
#include <windows.h>

const char START_CZONE[] = "CZONE1.MNI";
const char START_BGDROP[] = "DROP5.MNI";
const char START_MUSIC[] = "KICKBUTA.IMF";
const int START_LEVELWIDTH = 256;
const int START_FLAGS = LF_SCROLLHORZ;

extern Duke2Map Level;
extern CMP_File CMP;
extern WORD CurrentSolidTile, CurrentActor, CZoneAttributes[3600];
extern BYTE CurrentMaskedTile;
extern POINT BrushSizeSolid, BrushSizeMasked, BrushSize;
extern BOOL DrawActors, DrawMasked, DrawTiles, DrawGrid, DrawHoverRectangle;

void LoadCZone(const char*);
void DrawMap(POINT*, POINT*, POINT*);
void DrawFloatingSelection(SelectCopyPasteClass*, POINT*);
void DrawHoverRect(POINT*);
void DrawSolidTiles(int, int);
void DrawMaskedTiles(BOOL);
void InitGFX(POINT*);
void ShutDownEditor(void);
void ReloadGFX(POINT*);
void GetTileAttributesString(WORD, char* Dst);

#endif