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