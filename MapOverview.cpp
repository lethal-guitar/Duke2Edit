#include "MapOverview.h"
#include "EditorGFX.h"
#include "Gui.h"
#include "Map.h"
#include "resource.h"

static HBITMAP hBmLevelOverview;
static POINT LevelSize;

void AdjustDialogSize(HWND hDlg, int SizeX, int SizeY)
{
  RECT r = {0, 0, SizeX, SizeY};
  AdjustWindowRectEx(
    &r, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_CLIENTEDGE | WS_EX_TOOLWINDOW);
  SetWindowPos(
    hDlg, NULL, 0, 0, r.right + 6, r.bottom + 25, SWP_NOMOVE | SWP_NOZORDER);
}

HBITMAP CreateLevelOverview(Duke2Map* Level)
{
  struct
  {
    BITMAPINFOHEADER bmih;
    RGBQUAD bmiColors[16];
  } bmi;

  ZeroMemory(&bmi, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 2);
  bmi.bmih.biSize = sizeof(BITMAPINFOHEADER);
  bmi.bmih.biWidth = Level->GetLevelWidth();
  bmi.bmih.biHeight = Level->GetLevelHeight();
  bmi.bmih.biBitCount = 4;
  bmi.bmih.biPlanes = 1;
  bmi.bmih.biCompression = BI_RGB;
  bmi.bmiColors[0] = TILECOLOR_NULL;
  bmi.bmiColors[1] = TILECOLOR_BG;
  bmi.bmiColors[2] = TILECOLOR_SOLID;
  bmi.bmiColors[3] = TILECOLOR_CLIMB;

  BYTE* Data = new BYTE[(bmi.bmih.biWidth * bmi.bmih.biHeight) / 2];
  int DataIndex = 0;

  for (int y = bmi.bmih.biHeight - 1; y >= 0; y--)
  {
    for (int x = 0; x < bmi.bmih.biWidth; x++)
    {
      BYTE CurrentPixel = 0;

      if (Level->LevelData[y][x]->Solid)
        CurrentPixel = 1;

      if (
        CZoneAttributes[Level->LevelData[y][x]->Solid] & TA_SOLIDTOP ||
        CZoneAttributes[Level->LevelData[y][x]->Solid] & TA_SOLIDBOTTOM ||
        CZoneAttributes[Level->LevelData[y][x]->Solid] & TA_SOLIDLEFT ||
        CZoneAttributes[Level->LevelData[y][x]->Solid] & TA_SOLIDRIGHT)
        CurrentPixel = 2;

      if (
        CZoneAttributes[Level->LevelData[y][x]->Solid] & TA_LADDER ||
        CZoneAttributes[Level->LevelData[y][x]->Solid] & TA_HANGINGPOLE)
        CurrentPixel = 3;

      if (!(x % 2))
        Data[DataIndex] = CurrentPixel << 4;
      else
        Data[DataIndex++] |= CurrentPixel;
    }
  }

  HDC hDC = GetDC(NULL);
  HBITMAP Out = CreateDIBitmap(
    hDC, &bmi.bmih, CBM_INIT, Data, (BITMAPINFO*)&bmi, DIB_RGB_COLORS);
  ReleaseDC(NULL, hDC);
  delete Data;

  return Out;
}

BOOL CALLBACK
  MapOverviewDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg)
  {
    case WM_INITDIALOG:
      AdjustDialogSize(
        hDlg,
        (LevelSize.x = ((Duke2Map*)lParam)->GetLevelWidth()),
        LevelSize.y = (((Duke2Map*)lParam)->GetLevelHeight()));
      hBmLevelOverview = CreateLevelOverview((Duke2Map*)lParam);
      break;

    case WM_SIZE:
      InvalidateRect(hDlg, NULL, FALSE);
      break;

    case WM_PAINT:
      PAINTSTRUCT ps;
      BeginPaint(hDlg, &ps);

      {
        RECT r;
        GetClientRect(hDlg, &r);
        HDC hDCMem = CreateCompatibleDC(ps.hdc);
        HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));

        SelectObject(hDCMem, hBmLevelOverview);
        SelectObject(hDCMem, GetStockObject(HOLLOW_BRUSH));
        SelectObject(hDCMem, hPen);

        Rectangle(
          hDCMem,
          Gui.GetScrollPos().x,
          Gui.GetScrollPos().y,
          Gui.GetScrollPos().x + Gui.GetScrollPage().x,
          Gui.GetScrollPos().y + Gui.GetScrollPage().y);
        StretchBlt(
          ps.hdc,
          0,
          0,
          r.right,
          r.bottom,
          hDCMem,
          0,
          0,
          LevelSize.x,
          LevelSize.y,
          SRCCOPY);

        DeleteObject(hPen);
        DeleteDC(hDCMem);
      }
      EndPaint(hDlg, &ps);
      break;

    case WM_CLOSE:
      DeleteObject(hBmLevelOverview);
      EndDialog(hDlg, TRUE);
      break;

    default:
      return FALSE;
  }

  return TRUE;
}