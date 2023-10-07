#include "EditorGFX.h"
#include "D3D.h"
#include "Shared.h"
#include "resource.h"

Duke2Map Level;
CMP_File CMP;

static LPDIRECT3DSURFACE9 CZoneGFX, CZoneGridGFX, ActorSymbol, CZoneDrawSurface,
  GrayedOut;
static LPDIRECT3DTEXTURE9 CZoneTex, GridTex;
static char LastCZone[13];
static POINT GridRepetition, GridSize;
WORD CZoneAttributes[3600];

WORD CurrentSolidTile, CurrentActor;
BYTE CurrentMaskedTile;
BOOL DrawMasked = TRUE, DrawActors = TRUE, DrawTiles = TRUE, DrawGrid,
     DrawHoverRectangle = TRUE;
POINT BrushSizeSolid = {1, 1}, BrushSizeMasked = {1, 1}, BrushSize;

const int SELRECT_COLOR = 0xFF00FF00;
const int HOVERRECT_COLOR = 0xFFFFFF00;
const int GRID_COLOR = 0xFFFFFFFF;

void DrawFloatingSelection(SelectCopyPasteClass* SCP, POINT* ScrollPos)
{
  RECT CellsPos = {
    SCP->FocusRect.left / TILESIZE_EDITOR,
    SCP->FocusRect.top / TILESIZE_EDITOR,
    SCP->FocusRect.right / TILESIZE_EDITOR,
    SCP->FocusRect.bottom / TILESIZE_EDITOR};
  // RECT CellsPos = { SCP->FloatingSelDst.left/TILESIZE_EDITOR,
  // SCP->FloatingSelDst.top/TILESIZE_EDITOR,
  // SCP->FloatingSelDst.right/TILESIZE_EDITOR,
  // SCP->FloatingSelDst.bottom/TILESIZE_EDITOR };
  RECT rSrc, rDst;
  D3D.BeginSprite();

  OffsetRect(
    &CellsPos,
    -(ScrollPos->x - SCP->StartScrollPos.x),
    -(ScrollPos->y - SCP->StartScrollPos.y));
  // OffsetRect(&CellsPos, SCP->StartScrollPos.x, SCP->StartScrollPos.y);

  for (int y = CellsPos.top; y < CellsPos.bottom; y++)
  {
    for (int x = CellsPos.left; x < CellsPos.right; x++)
    {
      if (DrawTiles)
      {
        BYTE TileIndexY, TileIndexX;

        TileIndexY = SCP
                       ->FloatingSelection
                         [x - CellsPos.left +
                          (y - CellsPos.top) * (CellsPos.right - CellsPos.left)]
                       .Solid /
          40;
        TileIndexX = SCP
                       ->FloatingSelection
                         [x - CellsPos.left +
                          (y - CellsPos.top) * (CellsPos.right - CellsPos.left)]
                       .Solid %
          40;

        rSrc.left = TILESIZE_GAME * TileIndexX;
        rSrc.top = TILESIZE_GAME * TileIndexY;
        rSrc.right = rSrc.left + TILESIZE_GAME;
        rSrc.bottom = rSrc.top + TILESIZE_GAME;

        int xp = x;
        int yp = y;

        SetRect(
          &rDst,
          xp * TILESIZE_EDITOR,
          yp * TILESIZE_EDITOR,
          xp * TILESIZE_EDITOR + TILESIZE_EDITOR,
          yp * TILESIZE_EDITOR + TILESIZE_EDITOR);

        D3DXVECTOR2 Scaling = D3DXVECTOR2(1.0f, 1.0f);
        D3DXVECTOR2 Translation = D3DXVECTOR2(rDst.left, rDst.top);

        D3D.StretchRectToBackBuffer(CZoneGFX, &rSrc, &rDst);

        if (
          DrawMasked &&
          SCP
            ->FloatingSelection
              [x - CellsPos.left +
               (y - CellsPos.top) * (CellsPos.right - CellsPos.left)]
            .bMasked)
        {
          TileIndexY = 25 +
            SCP->FloatingSelection
                [x - CellsPos.left +
                 (y - CellsPos.top) * (CellsPos.right - CellsPos.left)]
                  .Masked /
              40;
          TileIndexX =
            SCP
              ->FloatingSelection
                [x - CellsPos.left +
                 (y - CellsPos.top) * (CellsPos.right - CellsPos.left)]
              .Masked %
            40;

          rSrc.left = TILESIZE_EDITOR * TileIndexX;
          rSrc.top = TILESIZE_EDITOR * TileIndexY;
          rSrc.right = rSrc.left + TILESIZE_EDITOR;
          rSrc.bottom = rSrc.top + TILESIZE_EDITOR;

          D3D.DrawSprite(CZoneTex, &rSrc, &Translation, 0xFFFFFFFF);
        }
      }

      SetRect(&rSrc, 0, 0, 32, 32);

      if (
        SCP
          ->FloatingSelection
            [x - CellsPos.left +
             (y - CellsPos.top) * (CellsPos.right - CellsPos.left)]
          .Actor &&
        DrawActors)
        D3D.StretchRectToBackBuffer(ActorSymbol, &rSrc, &rDst);

      // SetRect(&rSrc, 0, 0, 32, 32);

      // if (Level.LevelData[y][x]->Actor && DrawActors)
      //	D3D.StretchRectToBackBuffer(ActorSymbol, &rSrc, &rDst);
    }
  }

  D3D.EndSprite();

  SetRect(
    &CellsPos,
    CellsPos.left * TILESIZE_EDITOR,
    CellsPos.top * TILESIZE_EDITOR,
    CellsPos.right * TILESIZE_EDITOR,
    CellsPos.bottom * TILESIZE_EDITOR);

  D3D.BeginLine();

  D3DXVECTOR2 Points[2] = {
    D3DXVECTOR2(CellsPos.left, CellsPos.top),
    D3DXVECTOR2(CellsPos.left, CellsPos.bottom)};
  D3D.DrawLine(Points, 2, SELRECT_COLOR);
  Points[1] = D3DXVECTOR2(CellsPos.right, CellsPos.top);
  D3D.DrawLine(Points, 2, SELRECT_COLOR);
  Points[0] = D3DXVECTOR2(CellsPos.right, CellsPos.bottom);
  D3D.DrawLine(Points, 2, SELRECT_COLOR);
  Points[1] = D3DXVECTOR2(CellsPos.left, CellsPos.bottom);
  D3D.DrawLine(Points, 2, SELRECT_COLOR);

  D3D.EndLine();
}

void DrawMap(POINT* ScrollPos, POINT* pPage, POINT* SelectedTile)
{
  POINT Page = {pPage->x, pPage->y};

  if (Page.x >= Level.LevelWidth)
    Page.x = Level.LevelWidth;
  if (Page.y >= Level.GetLevelHeight())
    Page.y = Level.GetLevelHeight();

  RECT rSrc, rDst;

  D3D.BeginSprite();

  for (int y = ScrollPos->y; y < ScrollPos->y + Page.y; y++)
  {
    for (int x = ScrollPos->x; x < ScrollPos->x + Page.x; x++)
    {
      if (DrawTiles)
      {
        BYTE TileIndexY, TileIndexX;

        TileIndexY = Level.LevelData[y][x]->Solid / 40;
        TileIndexX = Level.LevelData[y][x]->Solid % 40;

        rSrc.left = TILESIZE_GAME * TileIndexX;
        rSrc.top = TILESIZE_GAME * TileIndexY;
        rSrc.right = rSrc.left + TILESIZE_GAME;
        rSrc.bottom = rSrc.top + TILESIZE_GAME;

        int xp = x - ScrollPos->x;
        int yp = y - ScrollPos->y;

        SetRect(
          &rDst,
          xp * TILESIZE_EDITOR,
          yp * TILESIZE_EDITOR,
          xp * TILESIZE_EDITOR + TILESIZE_EDITOR,
          yp * TILESIZE_EDITOR + TILESIZE_EDITOR);

        D3DXVECTOR2 Scaling = D3DXVECTOR2(1.0f, 1.0f);
        D3DXVECTOR2 Translation = D3DXVECTOR2(rDst.left, rDst.top);

        /*
        if (DrawGrid)
                D3D.StretchRectToBackBuffer(CZoneGridGFX, &rSrc, &rDst);
        else*/
        D3D.StretchRectToBackBuffer(CZoneGFX, &rSrc, &rDst);

        if (DrawMasked && Level.LevelData[y][x]->bMasked)
        {
          TileIndexY = 25 + Level.LevelData[y][x]->Masked / 40;
          TileIndexX = Level.LevelData[y][x]->Masked % 40;

          rSrc.left = TILESIZE_EDITOR * TileIndexX;
          rSrc.top = TILESIZE_EDITOR * TileIndexY;
          rSrc.right = rSrc.left + TILESIZE_EDITOR;
          rSrc.bottom = rSrc.top + TILESIZE_EDITOR;

          D3D.DrawSprite(CZoneTex, &rSrc, &Translation, 0xFFFFFFFF);
        }
      }

      if (
        SelectedTile->x + ScrollPos->x == x &&
        SelectedTile->y + ScrollPos->y == y)
        SetRect(&rSrc, 32, 0, 64, 32);
      else
        SetRect(&rSrc, 0, 0, 32, 32);

      if (Level.LevelData[y][x]->Actor && DrawActors)
        D3D.StretchRectToBackBuffer(ActorSymbol, &rSrc, &rDst);
    }
  }

  if (DrawGrid)
  {
    D3DXVECTOR2 Translation;

    for (auto y = 0; y < GridRepetition.y; y++)
    {
      for (int x = 0; x < GridRepetition.x; x++)
      {
        Translation = D3DXVECTOR2(x * GridSize.x, y * GridSize.y);
        D3D.DrawSprite(GridTex, nullptr, &Translation, 0xFFFFFFFF);
      }
    }
  }

  D3D.EndSprite();
}

void DrawHoverRect(POINT* HoverTile)
{
  RECT HoverRect = {
    HoverTile->x * TILESIZE_EDITOR,
    HoverTile->y * TILESIZE_EDITOR,
    BrushSize.x * TILESIZE_EDITOR,
    BrushSize.y * TILESIZE_EDITOR};
  HoverRect.right += HoverRect.left;
  HoverRect.bottom += HoverRect.top;

  D3D.BeginLine();

  D3DXVECTOR2 Points[2] = {
    D3DXVECTOR2(HoverRect.left, HoverRect.top),
    D3DXVECTOR2(HoverRect.left, HoverRect.bottom)};
  D3D.DrawLine(Points, 2, HOVERRECT_COLOR);
  Points[1] = D3DXVECTOR2(HoverRect.right, HoverRect.top);
  D3D.DrawLine(Points, 2, HOVERRECT_COLOR);
  Points[0] = D3DXVECTOR2(HoverRect.right, HoverRect.bottom);
  D3D.DrawLine(Points, 2, HOVERRECT_COLOR);
  Points[1] = D3DXVECTOR2(HoverRect.left, HoverRect.bottom);
  D3D.DrawLine(Points, 2, HOVERRECT_COLOR);

  D3D.EndLine();
}

void DrawSolidTiles(int Pos, int Page)
{
  RECT r = {0, 0, 320, 200};

  D3D.StretchRect(CZoneGFX, CZoneDrawSurface, &r);

  HDC hDC;

  CZoneDrawSurface->GetDC(&hDC);

  HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));

  SelectObject(hDC, GetStockObject(HOLLOW_BRUSH));
  SelectObject(hDC, hPen);
  r.left = CurrentSolidTile % 40 * TILESIZE_GAME;
  r.top = CurrentSolidTile / 40 * TILESIZE_GAME;
  r.right = r.left + TILESIZE_GAME + (BrushSizeSolid.x - 1) * TILESIZE_GAME;
  r.bottom = r.top + TILESIZE_GAME + (BrushSizeSolid.y - 1) * TILESIZE_GAME;
  POINT Offset = {r.right - 320, r.bottom - 200};
  Rectangle(
    hDC,
    r.left,
    r.top,
    r.right - ((Offset.x > 0) ? Offset.x : 0),
    r.bottom - ((Offset.y > 0) ? Offset.y : 0));
  DeleteObject(hPen);

  CZoneDrawSurface->ReleaseDC(hDC);

  SetRect(
    &r,
    0,
    Pos * TILESIZE_GAME,
    320,
    Pos * TILESIZE_GAME + Page * TILESIZE_GAME);

  D3D.StretchRectToBackBuffer(CZoneDrawSurface, &r);
}

void DrawMaskedTiles(BOOL GrayOutLast32)
{
  RECT r = {0, 200, 320, 232};
  RECT r2 = {0, 0, r.right - r.left, r.bottom - r.top};

  D3D.StretchRect(CZoneGFX, CZoneDrawSurface, &r, &r2);

  if (GrayOutLast32)
  {
    SetRect(&r, 64, 24, 320, 32);
    D3D.GetDevicePtr()->ColorFill(CZoneDrawSurface, &r, 0x77000000);
  }

  HDC hDC;

  CZoneDrawSurface->GetDC(&hDC);

  HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));

  SelectObject(hDC, GetStockObject(HOLLOW_BRUSH));
  SelectObject(hDC, hPen);
  r.left = CurrentMaskedTile % 40 * TILESIZE_GAME;
  r.top = CurrentMaskedTile / 40 * TILESIZE_GAME;
  r.right = r.left + TILESIZE_GAME + (BrushSizeMasked.x - 1) * TILESIZE_GAME;
  r.bottom = r.top + TILESIZE_GAME + (BrushSizeMasked.y - 1) * TILESIZE_GAME;
  POINT Offset = {r.right - 320, r.bottom - 232};
  Rectangle(
    hDC,
    r.left,
    r.top,
    r.right - ((Offset.x > 0) ? Offset.x : 0),
    r.bottom - ((Offset.y > 0) ? Offset.y : 0));
  DeleteObject(hPen);

  CZoneDrawSurface->ReleaseDC(hDC);

  D3D.StretchRectToBackBuffer(CZoneDrawSurface, &r2);
}

void LoadCZone(const char* FileName)
{
  if (CZoneGFX)
  {
    D3D.ReleaseSurface(CZoneGFX);
    CZoneGFX = NULL;
  }

  if (CZoneTex)
  {
    D3D.ReleaseTexture(LastCZone);
    CZoneTex = NULL;
  }

  LoadImageFromCMP(FileName, &CZoneGFX);
  LoadImageFromCMP(FileName, &CZoneTex);

  if (strcmp(FileName, LastCZone))
  {
    CMP.GetFile(FileName, (BYTE*)CZoneAttributes, 3600);
    strcpy(LastCZone, FileName);

    CurrentSolidTile = CurrentMaskedTile = 0;
    BrushSizeSolid.x = BrushSizeSolid.y = 1;
    BrushSizeMasked.x = BrushSizeMasked.y = 1;
    BrushSize = BrushSizeSolid;
  }
}

void GetTileAttributesString(WORD Tile, char* Dst)
{
  strset(Dst, 0);

  if (
    CZoneAttributes[Tile] & TA_SOLIDTOP ||
    CZoneAttributes[Tile] & TA_SOLIDBOTTOM ||
    CZoneAttributes[Tile] & TA_SOLIDLEFT ||
    CZoneAttributes[Tile] & TA_SOLIDRIGHT)
  {
    strcat(Dst, "Solid ");

    if (CZoneAttributes[Tile] & TA_SOLIDLEFT)
      strcat(Dst, "[ ");

    if (CZoneAttributes[Tile] & TA_SOLIDTOP)
      strcat(Dst, "^ ");

    if (CZoneAttributes[Tile] & TA_SOLIDBOTTOM)
      strcat(Dst, "_ ");

    if (CZoneAttributes[Tile] & TA_SOLIDRIGHT)
      strcat(Dst, "] ");

    Dst[strlen(Dst) - 1] = 0;
    strcat(Dst, ", ");
  }

  if (CZoneAttributes[Tile] & TA_ANIMATED)
    strcat(Dst, "Animated, ");

  if (CZoneAttributes[Tile] & TA_FOREGROUND)
    strcat(Dst, "Foreground, ");

  if (CZoneAttributes[Tile] & TA_DESTROYABLE)
    strcat(Dst, "Burnable, ");

  if (CZoneAttributes[Tile] & TA_HANGINGPOLE)
    strcat(Dst, "Hanging pole, ");

  if (CZoneAttributes[Tile] & TA_CONVEYORBELTL)
    strcat(Dst, "Conveyor Belt <-, ");

  if (CZoneAttributes[Tile] & TA_CONVEYORBELTL)
    strcat(Dst, "Conveyor Belt ->, ");

  if (CZoneAttributes[Tile] & TA_SLOWANIM)
    strcat(Dst, "Slow animation speed, ");

  if (CZoneAttributes[Tile] & TA_LADDER)
    strcat(Dst, "Ladder, ");

  if (CZoneAttributes[Tile] & TA_UNKNOWN1)
    strcat(Dst, "Unknown1, ");

  if (CZoneAttributes[Tile] & TA_UNKNOWN2)
    strcat(Dst, "Unknown2, ");

  if (CZoneAttributes[Tile] & TA_UNKNOWN3)
    strcat(Dst, "Unknown3, ");

  if (CZoneAttributes[Tile] & TA_UNKNOWN4)
    strcat(Dst, "Unknown4, ");

  Dst[strlen(Dst) - 2] = 0;
}

BOOL IsPowerOf2(long Val)
{
  int c = 31;

  for (int i = 31; i >= 0; i--, c--)
    if ((Val >> i) & 0x01)
      break;

  for (auto i = c; i >= 0; i--)
    if ((Val >> (i - 1)) & 0x01)
      return FALSE;

  return TRUE;
}

void CreateGridTex(POINT* ScrollPage)
{
  POINT Page = {ScrollPage->x / 2, ScrollPage->y / 2};

  while (!IsPowerOf2(Page.x * TILESIZE_EDITOR))
    Page.x++;
  while (!IsPowerOf2(Page.y * TILESIZE_EDITOR))
    Page.y++;

  GridRepetition.x = ScrollPage->x / Page.x + 1;
  GridRepetition.y = ScrollPage->y / Page.y + 1;
  GridSize.x = Page.x * TILESIZE_EDITOR;
  GridSize.y = Page.y * TILESIZE_EDITOR;

  GridTex = D3D.CreateTexture(
    "GridTex",
    Page.x * TILESIZE_EDITOR,
    Page.y * TILESIZE_EDITOR,
    1,
    0,
    D3DFMT_A8R8G8B8,
    D3DPOOL_DEFAULT);

  LPDIRECT3DTEXTURE9 Dummy = D3D.CreateTexture(
    "Dummy",
    Page.x * TILESIZE_EDITOR,
    Page.y * TILESIZE_EDITOR,
    1,
    0,
    D3DFMT_A8R8G8B8,
    D3DPOOL_SYSTEMMEM);

  D3DLOCKED_RECT LockedRect;
  Dummy->LockRect(0, &LockedRect, NULL, D3DLOCK_DISCARD);

  int Pitch = LockedRect.Pitch / 4;
  D3DCOLOR* Pixels = (D3DCOLOR*)LockedRect.pBits;

  for (int y = 0; y < Page.y * TILESIZE_EDITOR; y++)
    for (int x = 0; x < Page.x * TILESIZE_EDITOR; x++)
      Pixels[x + y * Pitch] =
        (!(x % TILESIZE_EDITOR) || !(y % TILESIZE_EDITOR)) ? GRID_COLOR : 0;

  Dummy->UnlockRect(0);

  D3D.GetDevicePtr()->UpdateTexture(Dummy, GridTex);
  D3D.ReleaseTexture("Dummy");
}

void InitGFX(POINT* ScrollPage)
{
  Level.Create(
    START_LEVELWIDTH,
    (char*)START_CZONE,
    (char*)START_MUSIC,
    (char*)START_BGDROP,
    START_FLAGS);
  LoadCZone(START_CZONE);
  ActorSymbol = D3D.CreateSurfaceFromResource(
    hInstance, MAKEINTRESOURCE(IDB_LEVELSYMBOLS), 0, 0, D3DFMT_X8R8G8B8);
  CZoneDrawSurface =
    D3D.CreateSurface(320, 200, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT);
  CreateGridTex(ScrollPage);

  strcpy(LastCZone, START_CZONE);
}

void ShutDownEditor(void)
{
  //
}

void ReloadGFX(POINT* ScrollPage)
{
  ActorSymbol = CZoneGFX = CZoneDrawSurface = NULL;
  CZoneTex = NULL;

  CZoneDrawSurface =
    D3D.CreateSurface(320, 200, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT);
  ActorSymbol = D3D.CreateSurfaceFromResource(
    hInstance, MAKEINTRESOURCE(IDB_LEVELSYMBOLS), 0, 0, D3DFMT_X8R8G8B8);
  LoadCZone(Level.GetCZone());
  CreateGridTex(ScrollPage);
}
