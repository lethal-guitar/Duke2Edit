#ifndef D3D_H
#define D3D_H

#pragma warning(disable : 4786)

#include <d3d9.h>
#include <d3dx9.h>
#include <map>
#include <string>

using namespace std;

const int MAX_SURFACES = 64;
const int MAX_TEXTURES = 64;
const int MAX_SWPCHAINS = 4;

extern class D3DManager
{
private:
  LPDIRECT3D9 lpD3D9;
  LPDIRECT3DDEVICE9 lpD3DDevice9;
  D3DCOLOR ClearColor;

  USHORT NrSurfacePtrs;
  LPDIRECT3DSURFACE9 lpBackBuffer;
  LPDIRECT3DSURFACE9 SurfacePtrs[MAX_SURFACES];
  // LPDIRECT3DSWAPCHAIN9 AdditionalSwapChains[MAX_SWPCHAINS];
  LPD3DXSPRITE lpSprite;
  LPD3DXLINE lpLine;
  map<string, LPDIRECT3DTEXTURE9> Textures;

  void AddSurfacePtr(LPDIRECT3DSURFACE9);
  void ReleaseAllSurfacePtrs(void);
  void ReleaseAllTextures(void);

public:
  ~D3DManager() { CleanUp(); }

  BOOL Init(HWND, HWND, BOOL, DWORD, DWORD, BOOL);
  void CleanUp(void);

  const LPDIRECT3DDEVICE9 GetDevicePtr(void) { return lpD3DDevice9; }

  void SetClearColor(D3DCOLOR NewColor) { ClearColor = NewColor; }
  BOOL Reset(HWND, BOOL, DWORD, DWORD);

  void BeginScene(void) { lpD3DDevice9->BeginScene(); }
  void BeginSceneClear(void)
  {
    lpD3DDevice9->Clear(0, 0, D3DCLEAR_TARGET, ClearColor, 1.0, 0);
    lpD3DDevice9->BeginScene();
  }
  HRESULT EndScene(HWND hWndDstOverride = NULL)
  {
    lpD3DDevice9->EndScene();
    return lpD3DDevice9->Present(NULL, NULL, hWndDstOverride, NULL);
  }

  BOOL CreateAdditionalSwapChain(HWND, BOOL, DWORD, DWORD);

  BOOL ReCreateSurfaceFromFile(
    LPDIRECT3DSURFACE9* ppSurface,
    BOOL Tiled,
    LPCTSTR FileName,
    DWORD dx,
    DWORD dy,
    D3DFORMAT Format,
    D3DPOOL Pool = D3DPOOL_DEFAULT,
    const RECT* pDstRect = NULL,
    const RECT* pSrcRect = NULL)
  {
    if (!ppSurface)
      return FALSE;
    if (*ppSurface)
      ReleaseSurface(*ppSurface);
    *ppSurface = CreateSurfaceFromFile(
      Tiled, FileName, dx, dy, Format, Pool, pDstRect, pSrcRect);
    if (!*ppSurface)
      return FALSE;
    else
      return TRUE;
  }
  const LPDIRECT3DSURFACE9 CreateSurfaceFromFile(
    BOOL Tiled,
    LPCTSTR pSrcFile,
    DWORD dx,
    DWORD dy,
    D3DFORMAT Format,
    D3DPOOL Pool = D3DPOOL_DEFAULT,
    const RECT* pDestRect = NULL,
    const RECT* pSrcRect = NULL);
  const LPDIRECT3DSURFACE9 CreateSurfaceFromResource(
    HINSTANCE hInstance,
    LPCTSTR Resource,
    DWORD dx,
    DWORD dy,
    D3DFORMAT Format,
    D3DPOOL Pool = D3DPOOL_DEFAULT,
    const RECT* pDestRect = NULL,
    const RECT* pSrcRect = NULL);
  const LPDIRECT3DSURFACE9
    CreateSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool);
  const LPDIRECT3DTEXTURE9
    CreateTexture(const char*, UINT, UINT, UINT, DWORD, D3DFORMAT, D3DPOOL);
  void ReleaseSurface(LPDIRECT3DSURFACE9);

  BOOL LoadTexture(
    const char*,
    UINT,
    UINT,
    UINT,
    DWORD,
    D3DFORMAT,
    D3DPOOL,
    DWORD,
    DWORD);
  void ReleaseTexture(const char*);
  const LPDIRECT3DTEXTURE9 GetTexture(const char*);

  HRESULT StretchRect(
    LPDIRECT3DSURFACE9 lpD3DSSrc,
    LPDIRECT3DSURFACE9 lpD3DSDst,
    const RECT* pSrcRect = NULL,
    const RECT* pDstRect = NULL,
    D3DTEXTUREFILTERTYPE Filter = D3DTEXF_NONE)
  {
    return lpD3DDevice9->StretchRect(
      lpD3DSSrc, pSrcRect, lpD3DSDst, pDstRect, Filter);
  }
  HRESULT StretchRectToBackBuffer(
    LPDIRECT3DSURFACE9 lpD3DS,
    const RECT* pSrcRect = NULL,
    const RECT* pDestRect = NULL,
    D3DTEXTUREFILTERTYPE Filter = D3DTEXF_NONE)
  {
    return lpD3DDevice9->StretchRect(
      lpD3DS, pSrcRect, lpBackBuffer, pDestRect, Filter);
  }
  void ColorFillRectBackBuffer(D3DCOLOR Cl, CONST RECT* pRect)
  {
    lpD3DDevice9->ColorFill(lpBackBuffer, pRect, Cl);
  }
  void DrawSprite(
    LPDIRECT3DTEXTURE9 pSrcTexture,
    CONST RECT* pSrcRect,
    CONST D3DXVECTOR2* pScaling,
    CONST D3DXVECTOR2* pRotationCenter,
    float Rotation,
    CONST D3DXVECTOR2* pTranslation,
    D3DCOLOR Color)
  {
    lpSprite->Draw(
      pSrcTexture,
      pSrcRect,
      pScaling,
      pRotationCenter,
      Rotation,
      pTranslation,
      Color);
  }
  void BeginSprite(void)
  {
    if (lpSprite)
      lpSprite->Begin();
  }
  void EndSprite(void)
  {
    if (lpSprite)
      lpSprite->End();
  }
  void DrawLine(
    const D3DXVECTOR2* pVertexList,
    DWORD dwVertexListCount,
    D3DCOLOR Color)
  {
    lpLine->Draw(pVertexList, dwVertexListCount, Color);
  }
  void BeginLine(void) { lpLine->Begin(); }
  void EndLine(void) { lpLine->End(); }
} D3D;

#endif