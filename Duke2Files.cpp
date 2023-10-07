#pragma warning(disable : 4786)

#include "Duke2Files.h"
#include "ActorImageDefs.h"
#include "EGASimulator.h"
#include "Log.h"
#include "Map.h"
#include <algorithm>
#include <list>

using namespace std;

static void ConvertHeaderStrings(LevelHeader* Header, BOOL ConvertBack = FALSE)
{
  if (ConvertBack)
  {
    for (auto i = 0; i < 12; i++)
    {
      if (!Header->CZone[i])
        Header->CZone[i] = 0x20;
    }

    for (auto i = 0; i < 12; i++)
    {
      if (!Header->BGDrop[i])
        Header->BGDrop[i] = 0x20;
    }

    for (auto i = 0; i < 12; i++)
    {
      if (!Header->Music[i])
        Header->Music[i] = 0x20;
    }

    return;
  }

  for (int i = 0; i < strlen(Header->CZone); i++)
  {
    if (isspace(Header->CZone[i]))
      Header->CZone[i] = 0;
  }

  for (auto i = 0; i < strlen(Header->BGDrop); i++)
  {
    if (isspace(Header->BGDrop[i]))
      Header->BGDrop[i] = 0;
  }

  for (auto i = 0; i < strlen(Header->Music); i++)
  {
    if (isspace(Header->Music[i]))
      Header->Music[i] = 0;
  }
}

static BOOL LoadImageToEGA(const char* Name, EGASimulator* EGA)
{
  int FileSize = CMP.GetFileSize(Name);
  int i = 0;

  if (!FileSize)
    return Error("LoadImageToEGA(): Failed to load file \"%s\"!", Name);

  BYTE* Data = new BYTE[FileSize];

  if (!CMP.GetFile(Name, Data))
  {
    delete Data;
    return Error("LoadImageToEGA(): Failed to load file \"%s\"!", Name);
  }

  PALENTRY* Palette = new PALENTRY[16];

  if (!CMP.GetFile("gamepal.pal", (BYTE*)Palette))
  {
    delete Palette;
    delete Data;
    return Error("LoadImageToEGA(): Failed to load palette \"gamepal.pal\"!");
  }

  EGA->LoadPalette(Palette);

  int DataIndex = 0;

  switch (CMP.DetermineFileType(Name))
  {
    case TYPE_BGDROP:
      EGA->BeginWrite(320, 200);

      for (i = 0; i < 25; i++)
      {
        for (int Col = 0; Col < 40; Col++)
        {
          for (int y = 0; y < 8; y++)
          {
            for (int Plane = 0; Plane < 4; Plane++)
            {
              EGA->SelectPlane(Plane);
              EGA->WriteByte(i * 320 + y * 40 + Col, Data[DataIndex++]);
            }
          }
        }
      }

      EGA->EndWrite();
      break;

    case TYPE_FULLSCREENIMG:
      EGA->BeginWrite(320, 200);

      for (i = 0; i < 4; i++)
      {
        EGA->SelectPlane(i);

        for (int y = 0; y < 100; y++)
          for (int x = 0; x < 80; x++)
            EGA->WriteByte(y * 80 + x, Data[DataIndex++]);
      }

      EGA->EndWrite();
      break;

    case TYPE_CZONE:
      DataIndex += CZONE_ATTRIBUTES_SIZE;

      EGA->BeginWrite(320, 232);

      for (i = 0; i < 25; i++)
      {
        for (int Col = 0; Col < 40; Col++)
        {
          for (int y = 0; y < 8; y++)
          {
            for (int Plane = 0; Plane < 4; Plane++)
            {
              EGA->SelectPlane(Plane);
              EGA->WriteByte(i * 320 + y * 40 + Col, Data[DataIndex++]);
            }
          }
        }
      }

      for (i = 0; i < 4; i++)
      {
        for (int Col = 0; Col < 40; Col++)
        {
          for (int y = 0; y < 8; y++)
          {
            for (int Plane = 0; Plane < 5; Plane++)
            {
              BYTE CurrentByte = Data[DataIndex++];

              if (Plane != 0)
              {
                EGA->SelectPlane(Plane - 1);
                EGA->WriteByte(i * 320 + y * 40 + Col + 8000, CurrentByte);
              }
              else
              {
                EGA->WriteMaskByte(i * 320 + y * 40 + Col + 8000, CurrentByte);
              }
            }
          }
        }
      }

      EGA->EndWrite();
      break;

    default:
      delete Palette;
      delete Data;
      return Error("LoadImageToEGA(): File \"%s\" is no valid image!", Name);
  }

  if (TYPE_FULLSCREENIMG == CMP.DetermineFileType(Name))
  {
    memcpy(Palette, Data + DataIndex, 48 * sizeof(BYTE));
    EGA->LoadPalette(Palette);
  }

  delete Palette;
  delete Data;

  return TRUE;
}

static BOOL LoadImageToEGA(WORD Id, EGASimulator* EGA)
{
  int ImgSize = (ActorImages[Id].x / 8) * (ActorImages[Id].y / 8) *
    ((ActorImages[Id].Masked) ? 40 : 36);

  BYTE* Data = new BYTE[ImgSize];

  if (!CMP.GetFile("ACTORS.MNI", Data, ImgSize, ActorImages[Id].Offset))
  {
    delete Data;
    return FALSE;
  }

  PALENTRY* Palette = new PALENTRY[16];

  if (!CMP.GetFile("gamepal.pal", (BYTE*)Palette))
  {
    delete Palette;
    delete Data;
    return Error("LoadImageToEGA(): Failed to load palette \"gamepal.pal\"!");
  }

  EGA->LoadPalette(Palette);
  EGA->BeginWrite(ActorImages[Id].x, ActorImages[Id].y);

  int DataIndex = 0;

  for (int Row = 0; Row < ActorImages[Id].y / 8; Row++)
  {
    for (int Col = 0; Col < ActorImages[Id].x / 8; Col++)
    {
      for (int y = 0; y < 8; y++)
      {
        for (int Plane = 0; Plane < 5; Plane++)
        {
          if (Plane != 0)
          {
            EGA->SelectPlane(Plane - 1);
            EGA->WriteByte(
              Row * ActorImages[Id].x + y * (ActorImages[Id].x / 8) + Col,
              Data[DataIndex]);
          }
          else
          {
            EGA->WriteMaskByte(
              Row * ActorImages[Id].x + y * (ActorImages[Id].x / 8) + Col,
              Data[DataIndex]);
          }

          DataIndex++;
        }
      }
    }
  }

  EGA->EndWrite();

  delete Palette;
  delete Data;

  return TRUE;
}

BOOL LoadMultipleActorImages(char* Images, HBITMAP* Dst)
{
  char CurrentStr[16] = {0};
  WORD ImgId;
  POINT PixOffset;
  EGASimulator* EGA = new EGASimulator;
  list<ImgDef> ImagesList;
  HDC hDC = GetDC(NULL);

  while (*Images != 0)
  {
    for (int i = 0; *Images != ','; i++, Images++)
      CurrentStr[i] = *Images;

    ImgId = atoi(CurrentStr);

    while (!isdigit(*Images))
      Images++;

    strset(CurrentStr, 0);

    for (auto i = 0; *Images != ','; i++, Images++)
      CurrentStr[i] = *Images;

    PixOffset.x = atoi(CurrentStr);

    while (!isdigit(*Images))
      Images++;

    strset(CurrentStr, 0);

    for (auto i = 0; *Images != ';' && *Images; i++, Images++)
      CurrentStr[i] = *Images;

    PixOffset.y = atoi(CurrentStr);

    strset(CurrentStr, 0);

    // while (!isdigit(*Images)) Images++;
    Images++;

    if (!LoadImageToEGA(ImgId, EGA))
    {
      delete EGA;
      return FALSE;
    }

    ImgDef Cur;

    Cur.Offset = PixOffset;
    Cur.Size.x = ActorImages[ImgId].x;
    Cur.Size.y = ActorImages[ImgId].y;
    Cur.ImgData = new DWORD[Cur.Size.x * Cur.Size.y];

    EGA->TransferImage(Cur.ImgData);

    // EGA->TransferImage(&Cur.Bitmap, hDC);
    // EGA->TransferMask(&Cur.Mask, hDC);

    ImagesList.push_back(Cur);
  }

  delete EGA;

  POINT ResultSize = ImagesList.front().Size;

  for (list<ImgDef>::iterator it = ImagesList.begin(); it != ImagesList.end();
       it++)
  {
    if (it->Offset.x * 8 + it->Size.x > ResultSize.x)
      ResultSize.x = it->Offset.x * 8 + it->Size.x;
    if (it->Offset.y * 8 + it->Size.y > ResultSize.y)
      ResultSize.y = it->Offset.y * 8 + it->Size.y;
  }

  DWORD* DestData = new DWORD[ResultSize.x * ResultSize.y];

  /*
  HDC     hDCMem  = CreateCompatibleDC(hDC);
  HDC     hDCMem2 = CreateCompatibleDC(hDC);
  HBITMAP Result = CreateCompatibleBitmap(hDC, ResultSize.x, ResultSize.y);
  ReleaseDC(NULL, hDC);

  SelectObject(hDCMem, Result);*/

  for (int i = 0; i < ResultSize.x * ResultSize.y; i++)
    DestData[i] = 0xAAAAAA;

  for (auto it = ImagesList.begin(); it != ImagesList.end(); it++)
  {
    int MaxX = it->Size.x + it->Offset.x * 8,
        MaxY = it->Size.y + it->Offset.y * 8;

    for (int y = it->Offset.y * 8; y < MaxY; y++)
    {
      for (int x = it->Offset.x * 8; x < MaxX; x++)
      {
        int in = x - it->Offset.x * 8 +
          (y - it->Offset.y * 8) * (MaxX - it->Offset.x * 8);

        DWORD CurrentDword = it->ImgData[in];

        if (CurrentDword & 0xFF000000)
          DestData[x + y * ResultSize.x] = CurrentDword;
      }
    }

    delete it->ImgData;

    /*
    //{

    //SelectObject(hDCMem2, it->Bitmap);
    //BitBlt(hDCMem, it->Offset.x*8, it->Offset.y*8, it->Size.x, it->Size.y,
    hDCMem2, 0, 0, SRCPAINT); SelectObject(hDCMem2, it->Bitmap); BitBlt(hDCMem,
    it->Offset.x*8, it->Offset.y*8, it->Size.x, it->Size.y, hDCMem2, 0, 0,
    SRCCOPY);
    //}
    //BitBlt (hdc, 0, 0, 100, 100 , hdcPic, 100, 0, 0x220326) ;
    //BitBlt (hdc, 0, 0, 100, 100 , hdcPic, 0, 0, SRCPAINT) ;
    //int ret = MaskBlt(hDCMem, it->Offset.x*8, it->Offset.y*8, it->Size.x,
    it->Size.y, hDCMem2, 0, 0, it->Mask, 0, 0, MAKEROP4(SRCCOPY,
    DSTINVERT0x00AA0029));

    DeleteObject(it->Bitmap);
    DeleteObject(it->Mask);*/
  }

  /*
  *Dst = Result;
  DeleteDC(hDCMem2);
  DeleteDC(hDCMem);
  */

  struct
  {
    BITMAPINFOHEADER bmih;
    RGBQUAD bmiColors[3];
  } bmi;

  ZeroMemory(&bmi, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 3);

  bmi.bmih.biSize = sizeof(BITMAPINFOHEADER);
  bmi.bmih.biWidth = ResultSize.x;
  bmi.bmih.biHeight = -ResultSize.y;
  bmi.bmih.biPlanes = 1;
  bmi.bmih.biBitCount = 32;
  bmi.bmih.biCompression = BI_RGB;

  *Dst = CreateDIBitmap(
    hDC, &bmi.bmih, CBM_INIT, DestData, LPBITMAPINFO(&bmi), DIB_RGB_COLORS);

  ReleaseDC(NULL, hDC);
  delete DestData;

  return TRUE;
}

BOOL LoadImageFromCMP(WORD Id, HBITMAP* Dst)
{
  EGASimulator* EGA = new EGASimulator;

  if (!LoadImageToEGA(Id, EGA))
  {
    delete EGA;
    return FALSE;
  }

  HDC hDC = GetDC(NULL);
  EGA->TransferImage(Dst, hDC);
  ReleaseDC(NULL, hDC);

  delete EGA;

  return TRUE;
}

BOOL LoadImageFromCMP(const char* Name, HBITMAP* Dst)
{
  EGASimulator* EGA = new EGASimulator;

  if (!LoadImageToEGA(Name, EGA))
  {
    delete EGA;
    return FALSE;
  }

  HDC hDC = GetDC(NULL);
  EGA->TransferImage(Dst, hDC);
  ReleaseDC(NULL, hDC);

  delete EGA;

  return TRUE;
}

BOOL LoadImageFromCMP(const char* Name, LPDIRECT3DSURFACE9* Dst, int dx, int dy)
{
  EGASimulator* EGA = new EGASimulator;

  if (!LoadImageToEGA(Name, EGA))
  {
    delete EGA;
    return FALSE;
  }

  int SizeY = 200;

  if (TYPE_CZONE == CMP.DetermineFileType(Name))
    SizeY += 32;

  LPDIRECT3DSURFACE9 Srf =
    D3D.CreateSurface(320, SizeY, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT);

  if (!Srf)
  {
    delete EGA;
    return Error("LoadImageFromCMP(): CreateSurface() failed!");
  }

  DWORD* ImageData = new DWORD[320 * SizeY];
  EGA->TransferImage(ImageData);

  D3DLOCKED_RECT LockedRect;
  Srf->LockRect(&LockedRect, NULL, 0);

  int Pitch = LockedRect.Pitch / 4;
  D3DCOLOR* Pixels = (D3DCOLOR*)LockedRect.pBits;

  for (int i = 0; i < 232; i++)
    for (int j = 0; j < 320; j++)
      Pixels[i * Pitch + j] = ImageData[i * 320 + j];

  Srf->UnlockRect();
  delete ImageData;
  delete EGA;

  /*
  if (dx || dy)
  {
          LPDIRECT3DSURFACE9 Tmp = D3D.CreateSurface(dx, dy, D3DFMT_A8R8G8B8,
  D3DPOOL_DEFAULT); D3D.StretchRect(Srf, Tmp); D3D.ReleaseSurface(Srf);

          *Dst = Tmp;

          return TRUE;
  }*/

  *Dst = Srf;

  return TRUE;
}

BOOL LoadImageFromCMP(const char* Name, LPDIRECT3DTEXTURE9* Dst)
{
  EGASimulator* EGA = new EGASimulator;

  if (!LoadImageToEGA(Name, EGA))
  {
    delete EGA;
    return FALSE;
  }

  int SizeY = 200;

  if (TYPE_CZONE == CMP.DetermineFileType(Name))
    SizeY += 32;

  LPDIRECT3DTEXTURE9 DstTex = D3D.CreateTexture(
    Name, 512 * 2, 256 * 2, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT);

  if (!DstTex)
    return FALSE;

  LPDIRECT3DTEXTURE9 Dummy = D3D.CreateTexture(
    "Dummy", 512 * 2, 256 * 2, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM);

  if (!Dummy)
  {
    D3D.ReleaseTexture(Name);
    return FALSE;
  }

  DWORD* ImageData = new DWORD[320 * SizeY];
  EGA->TransferImage(ImageData);

  D3DLOCKED_RECT LockedRect;
  Dummy->LockRect(0, &LockedRect, NULL, D3DLOCK_DISCARD);

  int Pitch = LockedRect.Pitch / 4;
  D3DCOLOR* Pixels = (D3DCOLOR*)LockedRect.pBits;

  for (int y = 0; y < SizeY; y++)
    for (int x = 0; x < 320; x++)
    {
      Pixels[y * 2 * Pitch + x * 2] = ImageData[y * 320 + x];
      Pixels[y * 2 * Pitch + x * 2 + 1] = ImageData[y * 320 + x];
      Pixels[y * 2 * Pitch + x * 2 + Pitch] = ImageData[y * 320 + x];
      Pixels[y * 2 * Pitch + x * 2 + 1 + Pitch] = ImageData[y * 320 + x];
    }

  Dummy->UnlockRect(0);

  delete ImageData;

  D3D.GetDevicePtr()->UpdateTexture(Dummy, DstTex);
  D3D.ReleaseTexture("Dummy");

  *Dst = DstTex;

  return TRUE;
}

static BOOL LoadLevelToEditor(BYTE* Level, Duke2Map* DstMap)
{
  LevelHeader Header = {0};
  memcpy(&Header.DataOffset, Level, sizeof(WORD));
  Level += 2;
  memcpy(&Header.CZone, Level, 13 * sizeof(char));
  Level += 13;
  memcpy(&Header.BGDrop, Level, 13 * sizeof(char));
  Level += 13;
  memcpy(&Header.Music, Level, 13 * sizeof(char));
  Level += 13;
  memcpy(&Header.Flags, Level, sizeof(WORD));
  Level += 2;
  memcpy(&Header.Unknown, Level, sizeof(WORD));
  Level += 2;
  memcpy(&Header.NrActorWords, Level, sizeof(WORD));
  Level += 2;

  int NrActors = (Header.DataOffset - 47 * sizeof(BYTE)) / sizeof(ActorDef);
  ActorDef* Actors = new ActorDef[NrActors];
  memcpy(Actors, Level, NrActors * sizeof(ActorDef));
  Level += NrActors * sizeof(ActorDef);

  WORD LevelWidth = 0;
  memcpy(&LevelWidth, Level, 2);
  Level += 2;

  WORD* LevelData = new WORD[LEVEL_DATA_WORDS];
  memcpy(LevelData, Level, LEVEL_DATA_WORDS * 2);
  Level += LEVEL_DATA_WORDS * 2;

  WORD ExtraInfoSize = 0;
  memcpy(&ExtraInfoSize, Level, 2);
  Level += 2;

  BYTE* ExtraInfo = new BYTE[ExtraInfoSize];
  memcpy(ExtraInfo, Level, ExtraInfoSize);
  Level += ExtraInfoSize;

  ConvertHeaderStrings(&Header);

  if (!CMP.GetFileSize(Header.CZone))
    return FALSE;

  if (!CMP.GetFileSize(Header.BGDrop))
    return FALSE;

  DstMap->Create(
    LevelWidth, Header.CZone, Header.Music, Header.BGDrop, Header.Flags);

  for (int i = 0; i < NrActors; i++)
  {
    POINT Pos = {Actors[i].x, Actors[i].y};

    if (Pos.x >= LevelWidth || Pos.y >= LEVEL_DATA_WORDS / LevelWidth)
      Error(
        "LoadLevelToEditor(): Invalid Actor Position: (%d, %d)!", Pos.x, Pos.y);
    else
      DstMap->SetActor(&Pos, Actors[i].Id);
  }

  for (int y = 0; y < DstMap->GetLevelHeight(); y++)
  {
    for (int x = 0; x < DstMap->GetLevelWidth(); x++)
    {
      WORD CurrentCell = LevelData[y * LevelWidth + x];
      POINT Pos = {x, y};

      if (CurrentCell >> 3 < 1000) // solid tile
      {
        DstMap->SetCellAttributes(&Pos, (CurrentCell >> 3), 0, FALSE);
      }
      else
      {
        if (!(CurrentCell & 0x8000)) // simple masked tile
        { // (solid tile #0 as background)
          DstMap->SetCellAttributes(
            &Pos, 0, ((CurrentCell >> 3) - 1000) / 5, TRUE);
        }
        else // complex (mixed masked/solid) masked tile
        {
          BYTE Tmp = HIBYTE(CurrentCell) ^ 0x80;

          int Shift = Tmp & 0x03;
          Tmp &= ~Shift;

          DstMap->SetCellAttributes(
            &Pos, LOBYTE(CurrentCell) + Shift * 256, Tmp >> 2, TRUE);
        }
      }
    }
  }

  int TileNrToModify = 0;
  auto i = 0;

  while (i < ExtraInfoSize)
  {
    if (i + 1 >= ExtraInfoSize)
    {
      if (ExtraInfo[i] != 0)
        WriteLogFile(
          "Warning: LoadCMPLevel(): Reached end of ExtraInfo before actual ending!\n");

      break;
    }

    LevelCell Cell;
    POINT Pos = {TileNrToModify % LevelWidth, TileNrToModify / LevelWidth};

    if (Pos.x >= LevelWidth)
    {
      Pos.x = 0;
      Pos.y++;
    }

    if (ExtraInfo[i] > 0x7F)
    {
      BYTE DataBytes = 0xFF - ExtraInfo[i++] + 1;
      BYTE Data = 0;

      for (int j = 0; j < DataBytes; j++)
      {
        Data = ExtraInfo[i++];

        for (int k = 0; k < 4; k++)
        {
          if (!DstMap->GetCellAttributes(&Pos, &Cell))
            Error(
              "Error: LoadCMPLevel(): GetCellAttributes() failed during read-out of tile shifting information! Pos: %d, %d",
              Pos.x,
              Pos.y);

          Cell.Masked += ((Data & (0x03 << k * 2)) >> k * 2) * 32;

          DstMap->SetCellAttributes(
            &Pos, Cell.Solid, Cell.Masked, Cell.bMasked);

          Pos.x++;

          if (Pos.x >= LevelWidth)
          {
            Pos.x = 0;
            Pos.y++;
          }
        }
      }

      TileNrToModify += DataBytes * 4;
    }

    if (ExtraInfo[i + 1] != 0)
    {
      for (int j = 0; j < ExtraInfo[i]; j++)
      {
        for (int k = 0; k < 4; k++)
        {
          if (!DstMap->GetCellAttributes(&Pos, &Cell))
            Error(
              "LoadCMPLevel(): GetCellAttributes() failed during read-out of tile shifting information! Pos: %d, %d",
              Pos.x,
              Pos.y);

          Cell.Masked += ((ExtraInfo[i + 1] & (0x03 << k * 2)) >> k * 2) * 32;

          DstMap->SetCellAttributes(
            &Pos, Cell.Solid, Cell.Masked, Cell.bMasked);

          Pos.x++;

          if (Pos.x >= LevelWidth)
          {
            Pos.x = 0;
            Pos.y++;
          }
        }
      }
    }

    TileNrToModify += ExtraInfo[i] * 4;
    i += 2;
  }

  delete[] ExtraInfo;
  delete[] LevelData;
  delete[] Actors;

  return TRUE;
}

BOOL LoadLevel(const char* FileName, Duke2Map* DstMap, BOOL LoadFromCMP)
{
  int Size = 0;
  FILE* fp = NULL;

  if (LoadFromCMP)
  {
    if (!(Size = CMP.GetFileSize(FileName)))
      return FALSE;
  }
  else
  {
    fp = fopen(FileName, "rb");

    if (!fp)
      return Error("LoadLevel(): Failed to open file \"%s\"!", FileName);

    fseek(fp, 0, SEEK_END);
    Size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
  }

  BYTE* Data = new BYTE[Size];

  if (LoadFromCMP)
  {
    if (!CMP.GetFile(FileName, Data))
    {
      delete Data;
      return Error("LoadLevel(): Failed to import file \"%s\"!", FileName);
    }
  }
  else
  {
    fread(Data, sizeof(BYTE), Size, fp);
    fclose(fp);
  }

  BOOL Res = LoadLevelToEditor(Data, DstMap);

  delete Data;

  return Res;
}

static void GenerateTSI(Duke2Map* Level, list<BYTE>* TSIData)
{
  POINT Pos;
  LevelCell Cell;
  TSIEntry Entry = {0}, LastEntry = {0};
  list<TSIEntry> EntrysCollected;

  BYTE TilesChecked = 0;
  int TilesSkipped = 0, TilesSkippedTotal = 0, NrEntrys = 0;
  BOOL PushBackEntrys = FALSE;

  for (int y = 0; y < Level->GetLevelHeight(); y++)
  {
    for (int x = 0; x < Level->GetLevelWidth(); x++)
    {
      // if (Level->GetLevelHeight()-1 == y && Level->GetLevelWidth() - x <= 3
      // && !EntrysCollected.empty()) 	goto NextEntryBlock;

      Pos.x = x;
      Pos.y = y;

      Level->GetCellAttributes(&Pos, &Cell);

      if (Cell.bMasked && Cell.Solid && Cell.Masked >= 32)
        Entry.Shifts[TilesChecked] = Cell.Masked / 32;

      TilesChecked++;

      if (4 == TilesChecked)
      {
        TilesChecked = 0;

        if (PushBackEntrys && !Entry.PackShifts())
        {
        NextEntryBlock:
          if (!EntrysCollected.front().Count)
          {
            int Diff = 0;

            list<TSIEntry>::iterator it;
            for (it = EntrysCollected.begin();
                 !(*it).Count && it != EntrysCollected.end();
                 it++, Diff++)
              ;

            if (it == EntrysCollected.end())
              Diff = 0;

            int NrShifts = (Diff) ? Diff : EntrysCollected.size();

            TSIData->push_back(0xFF - NrShifts + 1);

            TilesSkipped -= NrShifts;
          }

          while (EntrysCollected.size())
          {
            if (BYTE c = EntrysCollected.front().Count)
            {
              TilesSkipped -= c + 1;

              TSIData->push_back(c + 1);
              TSIData->push_back(EntrysCollected.front().PackShifts());
              EntrysCollected.pop_front();

              if (EntrysCollected.size())
                goto NextEntryBlock;
            }
            else
            {
              TSIData->push_back(EntrysCollected.front().PackShifts());
              EntrysCollected.pop_front();
            }
          }

          // if (Level->GetLevelHeight()-1 == y && Level->GetLevelWidth() - x <=
          // 3 && !EntrysCollected.empty()) 	continue;

          PushBackEntrys = FALSE;
        }

        if (Entry.PackShifts())
        {
          if (!LastEntry.PackShifts() && TilesSkipped && !PushBackEntrys)
          {
            if (0x30 == TilesSkipped)
              TilesSkipped = TilesSkipped;

            TSIData->push_back(TilesSkipped);
            TSIData->push_back(0);
            TilesSkippedTotal += TilesSkipped;
            TilesSkipped = 0;
          }

          if (
            !LastEntry.PackShifts() && PushBackEntrys &&
            !EntrysCollected.empty())
          {
            EntrysCollected.push_back(LastEntry);
            PushBackEntrys = FALSE;
            NrEntrys++;
          }

          if (
            !EntrysCollected.empty() &&
            Entry.PackShifts() == EntrysCollected.back().PackShifts())
            EntrysCollected.back().Count++;
          else
            EntrysCollected.push_back(Entry);

          NrEntrys++;
        }
        else if (!Entry.PackShifts() && LastEntry.PackShifts())
        {
          PushBackEntrys = TRUE;
        }

        TilesSkipped++;

        if (TilesSkipped == 0x7F)
        {
          TSIData->push_back(TilesSkipped);
          TSIData->push_back(0);
          TilesSkippedTotal += TilesSkipped;
          TilesSkipped = 0;
        }

        memcpy(&LastEntry, &Entry, sizeof(TSIEntry));
        ZeroMemory(&Entry, sizeof(TSIEntry));
      }
    }
  }


  if (!EntrysCollected.empty())
  {

  NextEntryBlock2:
    if (!EntrysCollected.front().Count)
    {
      int Diff = 0;

      list<TSIEntry>::iterator it; 
      for (it = EntrysCollected.begin();
           !(*it).Count && it != EntrysCollected.end();
           it++, Diff++)
        ;

      if (it == EntrysCollected.end())
        Diff = 0;

      int NrShifts = (Diff) ? Diff : EntrysCollected.size();

      TSIData->push_back(0xFF - NrShifts + 1);

      TilesSkipped -= NrShifts;
    }

    while (EntrysCollected.size())
    {
      if (BYTE c = EntrysCollected.front().Count)
      {
        TilesSkipped -= c + 1;

        TSIData->push_back(c + 1);
        TSIData->push_back(EntrysCollected.front().PackShifts());
        EntrysCollected.pop_front();

        if (EntrysCollected.size())
          goto NextEntryBlock2;
      }
      else
      {
        TSIData->push_back(EntrysCollected.front().PackShifts());
        EntrysCollected.pop_front();
      }
    }
  }

  TilesSkippedTotal += NrEntrys;

  if (TilesSkippedTotal < 8189)
  {
    while ((8189 - TilesSkippedTotal) / 0x7F)
    {
      TSIData->push_back(0x7F);
      TSIData->push_back(0);
      TilesSkippedTotal += 0x7F;
    }

    if (8189 - TilesSkippedTotal)
    {
      TSIData->push_back(8189 - TilesSkippedTotal);
      TSIData->push_back(0);
    }
  }

  TSIData->push_back(0);
}

BOOL SaveLevel(const char* FileName, Duke2Map* Level)
{
  LevelHeader Header = {0};

  strcpy(Header.BGDrop, Level->GetBGDrop());
  strcpy(Header.CZone, Level->GetCZone());
  strcpy(Header.Music, Level->GetMusic());

  ConvertHeaderStrings(&Header, TRUE);

  Header.DataOffset = 47 + sizeof(ActorDef) * Level->GetNrActors();
  Header.Flags = Level->GetFlags();
  Header.Unknown = 0;
  Header.NrActorWords = Level->GetNrActors() * 3;

  ActorDef* Actors = new ActorDef[Level->GetNrActors()];
  WORD* LevelData = new WORD[LEVEL_DATA_WORDS];

  ZeroMemory(LevelData, LEVEL_DATA_WORDS * sizeof(WORD));

  WORD w = Level->GetLevelWidth();
  POINT Pos = {0};
  LevelCell Cell;
  int id = 0, ia = 0;

  for (int y = 0; y < Level->GetLevelHeight(); y++)
  {
    for (int x = 0; x < Level->GetLevelWidth(); x++)
    {
      Pos.x = x;
      Pos.y = y;

      Level->GetCellAttributes(&Pos, &Cell);

      if (Cell.Actor)
      {
        Actors[ia].Id = Cell.ActorID;
        Actors[ia].x = x;
        Actors[ia++].y = y;
      }

      WORD Tile = 0;

      if (Cell.bMasked)
      {
        if (!Cell.Solid)
          Tile = (Cell.Masked * 5 + 1000) << 3;
        else
          Tile = 0x8000 |
            (((((Cell.Masked - 32 * (Cell.Masked / 32)) << 2) |
               (Cell.Solid / 256))
              << 8) |
             (Cell.Solid - 256 * (Cell.Solid / 256)));
      }
      else
        Tile = Cell.Solid << 3;

      LevelData[id++] = Tile;
    }
  }

  list<BYTE> TSIData;

  GenerateTSI(Level, &TSIData);

  FILE* fp = fopen(FileName, "wb");

  fwrite(&Header.DataOffset, sizeof(WORD), 1, fp);
  fwrite(&Header.CZone, sizeof(char), 13, fp);
  fwrite(&Header.BGDrop, sizeof(char), 13, fp);
  fwrite(&Header.Music, sizeof(char), 13, fp);
  fwrite(&Header.Flags, sizeof(WORD), 1, fp);
  fwrite(&Header.Unknown, sizeof(WORD), 1, fp);
  fwrite(&Header.NrActorWords, sizeof(WORD), 1, fp);
  fwrite(Actors, sizeof(ActorDef), Level->GetNrActors(), fp);
  fwrite(&w, sizeof(WORD), 1, fp);
  fwrite(LevelData, sizeof(WORD), LEVEL_DATA_WORDS, fp);
  w = TSIData.size();
  fwrite(&w, sizeof(WORD), 1, fp);

  BYTE* TSI = new BYTE[w];

  for (int i = 0; i < w; i++)
  {
    TSI[i] = TSIData.front();
    TSIData.pop_front();
  }

  fwrite(TSI, sizeof(BYTE), w, fp);

  char CZoneInfo[13] = {0};

  strcpy(CZoneInfo, Level->GetCZone() + 1);
  strcpy(CZoneInfo + 5, "atr.mni");
  fwrite(CZoneInfo, sizeof(char), 13, fp);

  strset(CZoneInfo + 5, 0x20);
  strcpy(CZoneInfo + 5, ".mni");
  CZoneInfo[5 + 4] = 0x20;
  fwrite(CZoneInfo, sizeof(char), 13, fp);

  strset(CZoneInfo + 5, 0x20);
  strcpy(CZoneInfo + 5, "msk.mni");
  fwrite(CZoneInfo, sizeof(char), 13, fp);
  fclose(fp);

  delete TSI;
  delete LevelData;
  delete Actors;

  return FALSE;
}