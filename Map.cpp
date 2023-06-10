#include "Map.h"
#include "Shared.h"

int Duke2Map::GetNrSolidTiles(void)
{
	int Ret = 0;
	
	for (int y=0; y<GetLevelHeight(); y++)
		for (int x=0; x<LevelWidth; x++)
			if (!LevelData[y][x]->bMasked) 
				Ret++;

	return Ret;
}

int Duke2Map::GetNrMaskedTiles(void)
{
	int Ret = 0;
	
	for (int y=0; y<GetLevelHeight(); y++)
		for (int x=0; x<LevelWidth; x++)
			if (LevelData[y][x]->bMasked) 
				Ret++;

	return Ret;
}

BOOL Duke2Map::GetCellAttributes(POINT* Pos, LevelCell* DstCell)
{
	if (!LevelWidth)
		return FALSE;

	if (Pos->x >= LevelWidth || Pos->y >= GetLevelHeight())
		return FALSE;

	memcpy(DstCell, LevelData[Pos->y][Pos->x], sizeof(LevelCell));

	return TRUE;
}

void Duke2Map::ChangeWidth(WORD NewWidth)
{
	for (int i=0; VALID_LEVEL_WIDTHS[i]; i++)
		if (NewWidth == VALID_LEVEL_WIDTHS[i]) 
			break;

	if (!VALID_LEVEL_WIDTHS[i] || NewWidth == LevelWidth)
		return;

	LevelCell*** TempData = new LevelCell**[GetLevelHeight()];

	for (i=0; i<GetLevelHeight(); i++)
	{
		TempData[i] = new LevelCell*[LevelWidth];

		for (int j=0; j<LevelWidth; j++)
		{
			LevelCell* pCell = new LevelCell;
			memcpy(pCell, LevelData[i][j], sizeof(LevelCell));
			TempData[i][j] = pCell;
		}
	}

	for (i=0; i<GetLevelHeight(); i++)
	{
		for (int j=0; j<LevelWidth; j++)
		{
			delete[] LevelData[i][j];
		}

		delete[] LevelData[i];
	}

	delete[] LevelData;

	NrActors = 0;

	LevelData = new LevelCell**[LEVEL_DATA_WORDS / NewWidth];

	for (i=0; i<LEVEL_DATA_WORDS / NewWidth; i++)
	{
		LevelData[i] = new LevelCell*[NewWidth];

		for (int j=0; j<NewWidth; j++)
		{
			LevelCell* pCell = new LevelCell;
			ZeroMemory(pCell, sizeof(LevelCell));

			LevelData[i][j] = pCell;
		}
	}
	
	for (i=0; i<LEVEL_DATA_WORDS / ((NewWidth < LevelWidth) ? LevelWidth : NewWidth); i++)
	{
		for (int j=0; j<((NewWidth < LevelWidth) ? NewWidth : LevelWidth); j++)
		{
			memcpy(LevelData[i][j], TempData[i][j], sizeof(LevelCell));
			
			if (TempData[i][j]->Actor)
				NrActors++;
		}
	}
	
	for (i=0; i<GetLevelHeight(); i++)
	{
		for (int j=0; j<LevelWidth; j++)
		{
			delete[] TempData[i][j];
		}

		delete[] TempData[i];
	}

	delete[] TempData;
	LevelWidth = NewWidth;
}

void Duke2Map::SetCellAttributes(POINT* Pos, WORD Solid, BYTE Masked, BOOL bMasked)
{
	if (!LevelWidth)
		return;

	if (Pos->x >= LevelWidth || Pos->y >= GetLevelHeight())
		return;
	
	LevelData[Pos->y][Pos->x]->Solid   = Solid;
	LevelData[Pos->y][Pos->x]->Masked  = Masked;
	LevelData[Pos->y][Pos->x]->bMasked = bMasked;
}

void Duke2Map::DeleteActor(POINT* Pos)
{
	if (!LevelWidth)
		return;

	if (Pos->x >= LevelWidth || Pos->y >= GetLevelHeight())
		return;

	if (LevelData[Pos->y][Pos->x]->Actor)
	{
		LevelData[Pos->y][Pos->x]->Actor = FALSE;
		NrActors--;
	}
}

void Duke2Map::CleanUp(void)
{
	if (!LevelWidth) 
		return;
	
	for (int i=0; i<GetLevelHeight(); i++)
	{
		for (int j=0; j<LevelWidth; j++)
		{
			delete[] LevelData[i][j];
		}

		delete[] LevelData[i];
	}

	delete[] LevelData;

	NrActors = LevelWidth = 0;
}

void Duke2Map::Create(int Width, char* CZone, char* Music, char* BGDrop, WORD Flags)
{
	CleanUp();
	
	strcpy(this->CZone, CZone);
	strcpy(this->Music, Music);
	strcpy(this->BGDrop, BGDrop);

	LevelWidth  = Width;
	this->Flags = Flags;
	
	LevelData   = new LevelCell**[GetLevelHeight()];

	for (int i=0; i<GetLevelHeight(); i++)
	{
		LevelData[i] = new LevelCell*[LevelWidth];

		for (int j=0; j<LevelWidth; j++)
		{
			LevelCell* pCell = new LevelCell;
			ZeroMemory(pCell, sizeof(LevelCell));

			LevelData[i][j] = pCell;
		}
	}
}

BOOL Duke2Map::GetActor(POINT* Pos, WORD* pActor)
{
	if (!LevelWidth)
		return FALSE;

	if (Pos->x >= LevelWidth || Pos->y >= GetLevelHeight())
		return FALSE;

	if (LevelData[Pos->y][Pos->x]->Actor) 
		*pActor = LevelData[Pos->y][Pos->x]->ActorID;
	
	return LevelData[Pos->y][Pos->x]->Actor;
}
