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


/******************************************************

        Main.cpp

******************************************************/


// -------------------------------------------------------
// Includes
// -------------------------------------------------------
#include "D3D.h"
#include "EditorGFX.h"
#include "Gui.h"
#include "Log.h"
#include "Messages.h"
#include "Shared.h"
#include "resource.h"
#include <string.h>
#include <windows.h>

// -------------------------------------------------------
// Constants
// -------------------------------------------------------
const char WNDCLASSNAME[] = "Duke2EditClass";
const char WNDTITLE[] = "Unnamed - ";
const int POS_X = CW_USEDEFAULT;
const int POS_Y = CW_USEDEFAULT;
const int SIZE_X = CW_USEDEFAULT;
const int SIZE_Y = CW_USEDEFAULT;

// -------------------------------------------------------
// Variables
// -------------------------------------------------------
HWND hWnd;
HINSTANCE hInstance;

// -------------------------------------------------------
// Functions
// -------------------------------------------------------
BOOL InitGUI(void)
{
  WNDCLASSEX wc;
  ZeroMemory(&wc, sizeof(WNDCLASSEX));

  wc.cbSize = sizeof(WNDCLASSEX);
  wc.lpfnWndProc = MainWindowProc;
  wc.hInstance = hInstance;
  wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DUKE2));
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = HBRUSH(COLOR_BTNFACE + 1);
  wc.lpszMenuName = MAKEINTRESOURCE(IDR_MAINMENU);
  wc.lpszClassName = WNDCLASSNAME;
  wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DUKE2));

  if (!RegisterClassEx(&wc))
  {
    Error("InitGUI(): Failed to register main window class!");
    return FALSE;
  }

  string WindowTitle = WNDTITLE;
  WindowTitle += APPNAME;

  hWnd = CreateWindowEx(
    0,
    WNDCLASSNAME,
    WindowTitle.c_str(),
    WS_OVERLAPPEDWINDOW | WS_MAXIMIZE | WS_CLIPSIBLINGS,
    POS_X,
    POS_Y,
    SIZE_X,
    SIZE_Y,
    NULL,
    NULL,
    hInstance,
    NULL);

  if (!hWnd)
  {
    Error("InitGUI(): Failed to create main window!");
    return FALSE;
  }

  return Gui.Init();
}

int WINAPI WinMain(
  HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPSTR lpCmdLine,
  int nShowCmd)
{
  CreateMutex(NULL, FALSE, "Duke2EditInstanceCheckMutex");

  if (ERROR_ALREADY_EXISTS == GetLastError())
    return 0;

  ::hInstance = hInstance;

  OpenLogFile("Duke2Edit");

  if (!InitGUI())
    return 0;

  if (!D3D.Init(hWnd, Gui.GetDrawAreaWindow(), TRUE, 0, 0, TRUE))
    return 0;

  InitGFX(&Gui.GetScrollPage());
  InitMessages();

  ShowWindow(hWnd, nShowCmd);
  UpdateWindow(hWnd);

  HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR));

  MSG msg;
  ZeroMemory(&msg, sizeof(MSG));

  while (GetMessage(&msg, NULL, 0, 0))
  {
    if (!TranslateAccelerator(hWnd, hAccel, &msg))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  ShutDownEditor();
  CleanUpMessages();
  CloseLogFile();

  return msg.wParam;
}