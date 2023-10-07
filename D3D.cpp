#include "D3D.h"
#include "Log.h"
#include "Shared.h"

D3DManager D3D;

static const char* GetDxErrorString(HRESULT hr)
{
  static char buffer[129];
  FormatMessage(
    FORMAT_MESSAGE_FROM_SYSTEM, nullptr, hr, 0, buffer, 128, nullptr);

  return buffer;
}


BOOL D3DManager::Reset(HWND hWndDev, BOOL Windowed, DWORD ResX, DWORD ResY)
{
  if (NULL == lpD3D9)
    return Error("D3DManager::Reset(): D3DManager wasn't initalized yet!");

  if (NULL != lpBackBuffer)
  {
    lpBackBuffer->Release();
    lpBackBuffer = NULL;
  }

  lpSprite->OnLostDevice();
  lpLine->OnLostDevice();

  ReleaseAllSurfacePtrs();
  ReleaseAllTextures();

  D3DPRESENT_PARAMETERS pp;
  ZeroMemory(&pp, sizeof(D3DPRESENT_PARAMETERS));

  pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
  pp.hDeviceWindow = hWndDev;
  pp.Windowed = Windowed;
  pp.BackBufferWidth = ResX;
  pp.BackBufferHeight = ResY;
  pp.BackBufferFormat = (Windowed) ? D3DFMT_UNKNOWN : D3DFMT_A8R8G8B8;

  HRESULT hr = lpD3DDevice9->Reset(&pp);

  if (FAILED(hr))
    return FALSE;

  lpD3DDevice9->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &lpBackBuffer);
  lpSprite->OnResetDevice();
  lpLine->OnResetDevice();

  return TRUE;
}

void D3DManager::CleanUp(void)
{
  ReleaseAllSurfacePtrs();
  ReleaseAllTextures();

  if (NULL != lpLine)
  {
    lpLine->Release();
    lpLine = NULL;
  }

  if (NULL != lpSprite)
  {
    lpSprite->Release();
    lpSprite = NULL;
  }

  if (NULL != lpBackBuffer)
  {
    lpBackBuffer->Release();
    lpBackBuffer = NULL;
  }

  if (NULL != lpD3DDevice9)
  {
    lpD3DDevice9->Release();
    lpD3DDevice9 = NULL;
  }

  if (NULL != lpD3D9)
  {
    lpD3D9->Release();
    lpD3D9 = NULL;
  }
}

BOOL D3DManager::Init(
  HWND hWndFocus,
  HWND hWndDev,
  BOOL Windowed,
  DWORD ResX,
  DWORD ResY,
  BOOL CreateSprite)
{
  CleanUp();

  if (NULL == (lpD3D9 = Direct3DCreate9(D3D_SDK_VERSION)))
    return Error("D3DManager::Init(): Direct3DCreate9() failed!");

  D3DPRESENT_PARAMETERS pp;
  ZeroMemory(&pp, sizeof(D3DPRESENT_PARAMETERS));

  pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
  pp.hDeviceWindow = hWndDev;
  pp.Windowed = Windowed;
  pp.BackBufferWidth = ResX;
  pp.BackBufferHeight = ResY;
  pp.BackBufferFormat = (Windowed) ? D3DFMT_UNKNOWN : D3DFMT_A8R8G8B8;

  HRESULT hr;

  if (FAILED(
        hr = lpD3D9->CreateDevice(
          D3DADAPTER_DEFAULT,
          D3DDEVTYPE_HAL,
          hWndFocus,
          D3DCREATE_SOFTWARE_VERTEXPROCESSING,
          &pp,
          &lpD3DDevice9)))
  {
    lpD3D9->Release();
    lpD3D9 = NULL;
    return Error(
      "D3DManager::Init(): IDirect3D9::CreateDevice() failed! Cause: %s",
      GetDxErrorString(hr));
  }

  lpD3DDevice9->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &lpBackBuffer);

  if (CreateSprite)
  {
    if (FAILED(hr = D3DXCreateSprite(lpD3DDevice9, &lpSprite)))
    {
      CleanUp();
      return Error(
        "D3DManager::Init(): D3DXCreateSprite() failed! Cause: %s",
        GetDxErrorString(hr));
    }
  }

  if (FAILED(hr = D3DXCreateLine(lpD3DDevice9, &lpLine)))
  {
    CleanUp();
    return Error(
      "D3DManager::Init(): D3DXCreateLine() failed! Cause: %s",
      GetDxErrorString(hr));
  }

  lpLine->SetAntialias(FALSE);
  lpLine->SetWidth(2);

  return TRUE;
}

void D3DManager::AddSurfacePtr(LPDIRECT3DSURFACE9 lpD3DS)
{
  if (NULL == lpD3D9)
  {
    Error("D3DManager::AddSurfacePtr(): D3DManager wasn't initalized yet!");
    return;
  }

  if (!lpD3DS)
  {
    Error("D3DManager::AddSurfacePtr(): Argument is not valid (NULL)");
    return;
  }

  if (NrSurfacePtrs == MAX_SURFACES)
  {
    Error(
      "D3DManager::AddSurfacePtr(): Max surface count (%d) reached, surface pointer not added",
      MAX_SURFACES);
    return;
  }

  SurfacePtrs[NrSurfacePtrs++] = lpD3DS;
}

const LPDIRECT3DSURFACE9 D3DManager::CreateSurface(
  UINT Width,
  UINT Height,
  D3DFORMAT Format,
  D3DPOOL Pool)
{
  if (NULL == lpD3D9)
  {
    Error("D3DManager::CreateSurface(): D3DManager wasn't initalized yet!");
    return NULL;
  }

  if (NrSurfacePtrs == MAX_SURFACES)
  {
    Error(
      "D3DManager::CreateSurface(): Max surface count (%d) reached, surface not created",
      MAX_SURFACES);
    return NULL;
  }

  LPDIRECT3DSURFACE9 lpD3DS = NULL;

  HRESULT hr = lpD3DDevice9->CreateOffscreenPlainSurface(
    Width, Height, Format, Pool, &lpD3DS, NULL);

  if (FAILED(hr))
  {
    Error(
      "D3DManager::CreateSurface(): CreateOffScreenPlainSurface() failed! Cause: %s",
      GetDxErrorString(hr));
    return NULL;
  }

  AddSurfacePtr(lpD3DS);
  return lpD3DS;
}

const LPDIRECT3DSURFACE9 D3DManager::CreateSurfaceFromFile(
  BOOL Tiled,
  LPCTSTR pSrcFile,
  DWORD dx,
  DWORD dy,
  D3DFORMAT Format,
  D3DPOOL Pool,
  const RECT* pDestRect,
  const RECT* pSrcRect)
{
  if (NULL == lpD3D9)
  {
    Error(
      "D3DManager::CreateSurfaceFromFile(): D3DManager wasn't initalized yet!");
    return NULL;
  }

  if (NrSurfacePtrs == MAX_SURFACES)
  {
    Error(
      "D3DManager::CreateSurfaceFromFile(): Max surface count (%d) reached, surface not created",
      MAX_SURFACES);
    return NULL;
  }

  D3DXIMAGE_INFO Info;

  HRESULT hr = D3DXGetImageInfoFromFile(pSrcFile, &Info);

  if (FAILED(hr))
  {
    Error(
      "D3DManager::CreateSurfaceFromFile(): D3DXGetImageInfoFromFile() failed on file "
      "%s"
      ", hr = %X",
      pSrcFile,
      hr);
    return NULL;
  }

  LPDIRECT3DSURFACE9 lpD3DS = NULL;

  if (!dx)
    dx = Info.Width;
  if (!dy)
    dy = Info.Height;

  hr = lpD3DDevice9->CreateOffscreenPlainSurface(
    dx, dy, Format, Pool, &lpD3DS, NULL);

  if (FAILED(hr))
  {
    Error(
      "D3DManager::CreateSurfaceFromFile(): CreateOffScreenPlainSurface() failed! Cause: %s",
      GetDxErrorString(hr));
    return NULL;
  }

  if (Tiled)
  {
    LPDIRECT3DSURFACE9 lpD3DSTmp = NULL;

    hr = lpD3DDevice9->CreateOffscreenPlainSurface(
      Info.Width, Info.Height, Format, Pool, &lpD3DSTmp, NULL);

    if (FAILED(hr))
    {
      lpD3DS->Release();
      Error(
        "D3DManager::CreateSurfaceFromFile(): CreateOffScreenPlainSurface() failed for temporary surface! Cause: %s",
        GetDxErrorString(hr));
      return NULL;
    }

    hr = D3DXLoadSurfaceFromFile(
      lpD3DSTmp, NULL, NULL, pSrcFile, NULL, D3DX_DEFAULT, 0, NULL);

    if (FAILED(hr))
    {
      lpD3DS->Release();
      lpD3DSTmp->Release();
      Error(
        "D3DManager::CreateSurfaceFromFile(): D3DXLoadSurfaceFromFile failed on file "
        "%s"
        " for temporary surface! Cause: %s",
        GetDxErrorString(hr));
      return NULL;
    }

    for (DWORD i = 0; i <= dy / Info.Height; i++)
    {
      for (DWORD j = 0; j <= dx / Info.Width; j++)
      {
        RECT r = {0, 0, Info.Width, Info.Height};
        RECT r2 = {
          j * Info.Width,
          i * Info.Height,
          Info.Width + j * Info.Width,
          Info.Height + i * Info.Height};

        if (j == dx / Info.Width)
        {
          r.right = dx % Info.Width;
          r2.right = dx;
        }

        if (i == dy / Info.Height)
        {
          r.bottom = dy % Info.Height;
          r2.bottom = dy;
        }

        lpD3DDevice9->StretchRect(lpD3DSTmp, &r, lpD3DS, &r2, D3DTEXF_NONE);
      }
    }

    lpD3DSTmp->Release();
  }
  else
  {
    hr = D3DXLoadSurfaceFromFile(
      lpD3DS, NULL, pDestRect, pSrcFile, pSrcRect, D3DX_DEFAULT, 0, NULL);

    if (FAILED(hr))
    {
      lpD3DS->Release();
      Error(
        "D3DManager::CreateSurfaceFromFile(): D3DXLoadSurfaceFromFile failed on file "
        "%s"
        "!Cause: %s",
        GetDxErrorString(hr));
      return NULL;
    }
  }

  AddSurfacePtr(lpD3DS);
  return lpD3DS;
}

const LPDIRECT3DSURFACE9 D3DManager::CreateSurfaceFromResource(
  HINSTANCE hInstance,
  LPCTSTR pSrcFile,
  DWORD dx,
  DWORD dy,
  D3DFORMAT Format,
  D3DPOOL Pool,
  const RECT* pDestRect,
  const RECT* pSrcRect)
{
  if (NULL == lpD3D9)
  {
    Error(
      "D3DManager::CreateSurfaceFromResource(): D3DManager wasn't initalized yet!");
    return NULL;
  }

  if (NrSurfacePtrs == MAX_SURFACES)
  {
    Error(
      "D3DManager::CreateSurfaceFromResource(): Max surface count (%d) reached, surface not created",
      MAX_SURFACES);
    return NULL;
  }

  /*
  D3DXIMAGE_INFO Info;

  HRESULT hr = D3DXGetImageInfoFromFile(pSrcFile, &Info);

  if (FAILED(hr))
  {
          Error("D3DManager::CreateSurfaceFromFile(): D3DXGetImageInfoFromFile()
  failed on file ""%s"", hr = %X", pSrcFile, hr); return NULL;
  }*/

  HBITMAP hBm =
    (HBITMAP)LoadImage(hInstance, pSrcFile, IMAGE_BITMAP, dx, dy, 0);

  if (!hBm)
  {
    Error("D3DManager::CreateSurfaceFromResource(): LoadImage() failed!");
    return NULL;
  }

  BITMAPINFO bi;
  ZeroMemory(&bi, sizeof(BITMAPINFO));
  bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

  HDC hDC = GetDC(NULL);
  GetDIBits(hDC, hBm, 0, 0, NULL, &bi, DIB_RGB_COLORS);

  LPDIRECT3DSURFACE9 lpD3DS = NULL;

  if (!dx)
    dx = bi.bmiHeader.biWidth;
  if (!dy)
    dy = bi.bmiHeader.biHeight;

  HRESULT hr = lpD3DDevice9->CreateOffscreenPlainSurface(
    dx, dy, Format, Pool, &lpD3DS, NULL);

  if (FAILED(hr))
  {
    Error(
      "D3DManager::CreateSurfaceFromResource(): CreateOffScreenPlainSurface() failed! Cause: %s",
      GetDxErrorString(hr));
    return NULL;
  }

  ReleaseDC(NULL, hDC);

  lpD3DS->GetDC(&hDC);

  HDC hDCMem = CreateCompatibleDC(hDC);
  SelectObject(hDCMem, hBm);
  BitBlt(hDC, 0, 0, dx, dy, hDCMem, 0, 0, SRCCOPY);
  DeleteDC(hDCMem);
  DeleteObject(hBm);
  lpD3DS->ReleaseDC(hDC);

  AddSurfacePtr(lpD3DS);
  return lpD3DS;
}

BOOL D3DManager::LoadTexture(
  const char* FileName,
  UINT Width,
  UINT Height,
  UINT MipLevels,
  DWORD Usage,
  D3DFORMAT Format,
  D3DPOOL Pool,
  DWORD Filter,
  DWORD MipFilter)
{
  if (NULL == lpD3D9)
    return Error(
      "D3DManager::LoadTexture(): D3DManager wasn't initalized yet!");

  map<string, LPDIRECT3DTEXTURE9>::iterator it = Textures.find(FileName);

  if (it != Textures.end())
    return FALSE;

  if (Textures.size() == MAX_TEXTURES)
  {
    return Error(
      "D3DManager::LoadTexture(): Max texture count (%d) reached, texture "
      "%s"
      " not loaded",
      MAX_TEXTURES,
      FileName);
  }

  HRESULT hr = D3DXCreateTextureFromFileEx(
    lpD3DDevice9,
    FileName,
    Width,
    Height,
    MipLevels,
    Usage,
    Format,
    Pool,
    Filter,
    MipFilter,
    0,
    NULL,
    NULL,
    &Textures[FileName]);

  if (FAILED(hr))
  {
    Textures.erase(FileName);
    return Error(
      "D3DManager::LoadTexture(): D3DXCreateTextureFromFileEx() failed on file "
      "%s"
      "! Cause: %s",
      GetDxErrorString(hr));
  }

  return TRUE;
}

const LPDIRECT3DTEXTURE9 D3DManager::CreateTexture(
  const char* Name,
  UINT Width,
  UINT Height,
  UINT Levels,
  DWORD Usage,
  D3DFORMAT Format,
  D3DPOOL Pool)
{
  if (NULL == lpD3D9)
  {
    Error("D3DManager::CreateTexture(): D3DManager wasn't initalized yet!");
    return NULL;
  }

  map<string, LPDIRECT3DTEXTURE9>::iterator it = Textures.find(Name);

  if (it != Textures.end())
    return NULL;

  if (Textures.size() == MAX_TEXTURES)
  {
    Error(
      "D3DManager::CreateTexture(): Max texture count (%d) reached, texture not created",
      MAX_TEXTURES);
    return NULL;
  }

  LPDIRECT3DTEXTURE9 Out;

  HRESULT hr = lpD3DDevice9->CreateTexture(
    Width, Height, Levels, Usage, Format, Pool, &Out, NULL);


  if (FAILED(hr))
  {
    Error(
      "D3DManager::CreateTexture(): CreateTexture() failed! Cause: %s",
      GetDxErrorString(hr));
    return NULL;
  }

  Textures[Name] = Out;

  return Out;
}

const LPDIRECT3DTEXTURE9 D3DManager::GetTexture(const char* Name)
{
  if (NULL == lpD3D9)
  {
    Error("D3DManager::GetTexture(): D3DManager wasn't initalized yet!");
    return NULL;
  }

  map<string, LPDIRECT3DTEXTURE9>::iterator it = Textures.find(Name);

  if (it == Textures.end())
  {
    Error(
      "D3DManager::GetTexture(): Not found: "
      "%s"
      "",
      Name);
    return NULL;
  }

  return (*it).second;
}

void D3DManager::ReleaseAllSurfacePtrs(void)
{
  for (USHORT i = 0; i < MAX_SURFACES; i++)
  {
    if (SurfacePtrs[i] != NULL)
    {
      SurfacePtrs[i]->Release();
      SurfacePtrs[i] = NULL;
    }
  }

  NrSurfacePtrs = 0;
}

void D3DManager::ReleaseAllTextures(void)
{
  for (map<string, LPDIRECT3DTEXTURE9>::iterator it = Textures.begin();
       it != Textures.end();
       it++)
  {
    if ((*it).second != NULL)
    {
      (*it).second->Release();
    }
  }

  Textures.clear();
}

void D3DManager::ReleaseSurface(LPDIRECT3DSURFACE9 lpD3DS)
{
  if (lpD3DS == NULL)
  {
    Error("D3DManager::ReleaseSurface(): Argument not valid (NULL)");
    return;
  }

  USHORT i;
  for (i = 0; i < NrSurfacePtrs; i++)
  {
    if (SurfacePtrs[i] == lpD3DS)
    {
      SurfacePtrs[i]->Release();
      SurfacePtrs[i] = NULL;
      break;
    }
  }

  if (i == NrSurfacePtrs)
  {
    Error("D3DManager::ReleaseSurface(): Not found: %X", lpD3DS);
    return;
  }

  if (i + 1 < MAX_SURFACES)
  {
    if (SurfacePtrs[i + 1] != NULL)
    {
      USHORT j;
      for (j = i; j < NrSurfacePtrs; j++)
      {
        SurfacePtrs[j] = SurfacePtrs[j + 1];
      }

      SurfacePtrs[j] = NULL;
    }
  }

  NrSurfacePtrs--;
}

void D3DManager::ReleaseTexture(const char* Name)
{
  map<string, LPDIRECT3DTEXTURE9>::iterator it = Textures.find(Name);

  if (it == Textures.end())
  {
    Error("D3DManager::ReleaseTexture(): Not found: %s", Name);
    return;
  }

  if ((*it).second != NULL)
  {
    (*it).second->Release();
  }

  Textures.erase(it);
}
