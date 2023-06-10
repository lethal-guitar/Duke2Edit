#ifndef DUKE2FILES_H
#define DUKE2FILES_H

#include <windows.h>
#include "D3D.h"
#include "CMP.h"

const int CZONE_ATTRIBUTES_SIZE = 3600;
const int LEVEL_DATA_WORDS	    = 32750;

extern CMP_File CMP;

enum TILE_ATTRIBUTES 
{
	TA_SOLIDTOP = 0x0001,
	TA_SOLIDBOTTOM = 0x0002,
	TA_SOLIDRIGHT = 0x0004,
	TA_SOLIDLEFT = 0x0008,
	TA_ANIMATED = 0x0010,
	TA_FOREGROUND = 0x0020,
	TA_DESTROYABLE = 0x0040,
	TA_HANGINGPOLE = 0x0080,
	TA_CONVEYORBELTL = 0x0100,
	TA_CONVEYORBELTR = 0x0200,
	TA_SLOWANIM = 0x0400,
	TA_UNKNOWN1 = 0x0800,
	TA_UNKNOWN2 = 0x1000,
	TA_UNKNOWN3 = 0x2000,
	TA_LADDER = 0x4000,
	TA_UNKNOWN4 = 0x8000
};

struct ImgDef
{
	//HBITMAP Bitmap, Mask;
	DWORD*  ImgData;
	POINT   Offset;
	POINT   Size;
};

struct LevelHeader
{
	WORD DataOffset;
	char CZone[13];
	char BGDrop[13];
	char Music[13];
	WORD Flags;
	WORD Unknown;
	WORD NrActorWords;
};

struct ActorDef
{
	WORD Id;
	WORD x;
	WORD y;
};

struct TSIEntry
{
	BYTE Count;
	BYTE Shifts[4];
	
	BYTE PackShifts(void) 
	{
		return Shifts[0] | (Shifts[1] << 2) | (Shifts[2] << 4) | (Shifts[3] << 6);
	}
};

class Duke2Map;

BOOL LoadImageFromCMP(const char* Name, HBITMAP* Dst);
BOOL LoadImageFromCMP(const char* Name, LPDIRECT3DSURFACE9* Dst, int dx = 0, int dy = 0);
BOOL LoadImageFromCMP(const char* Name, LPDIRECT3DTEXTURE9* Dst);
BOOL LoadImageFromCMP(WORD Id, HBITMAP* Dst);
BOOL LoadMultipleActorImages(char* Images, HBITMAP* Dst);
BOOL LoadLevel(const char* FileName, Duke2Map* DstMap, BOOL LoadFromCMP = FALSE);
BOOL SaveLevel(const char* FileName, Duke2Map*);

#endif