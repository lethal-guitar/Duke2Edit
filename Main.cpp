/******************************************************
	
	Main.cpp

******************************************************/


// -------------------------------------------------------
// Includes
// -------------------------------------------------------
#include <windows.h>
#include <string.h>
#include "D3D.h"
#include "Messages.h"
#include "EditorGFX.h"
#include "Gui.h"
#include "Shared.h"
#include "Log.h"
#include "resource.h"

// -------------------------------------------------------
// Constants
// -------------------------------------------------------
const char WNDCLASSNAME[] = "Duke2EditClass";
const char WNDTITLE[]     = "Unnamed - ";
const int  POS_X		  = CW_USEDEFAULT;
const int  POS_Y		  = CW_USEDEFAULT;
const int  SIZE_X		  = CW_USEDEFAULT;
const int  SIZE_Y		  = CW_USEDEFAULT;

// -------------------------------------------------------
// Variables
// -------------------------------------------------------
HWND      hWnd;
HINSTANCE hInstance;

// -------------------------------------------------------
//Functions
// -------------------------------------------------------
BOOL InitGUI(void)
{
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize        = sizeof(WNDCLASSEX);		
	wc.lpfnWndProc   = MainWindowProc;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DUKE2));
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = HBRUSH(COLOR_BTNFACE+1);
	wc.lpszMenuName  = MAKEINTRESOURCE(IDR_MAINMENU);
	wc.lpszClassName = WNDCLASSNAME; 
	wc.hIconSm       = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DUKE2)); 
	
	if (!RegisterClassEx(&wc)) 
	{
		Error("InitGUI(): Failed to register main window class!");
		return FALSE;
	}

	string WindowTitle = WNDTITLE;
	WindowTitle += APPNAME;
	
	hWnd = CreateWindowEx(0, 
					      WNDCLASSNAME, 
						  WindowTitle.c_str(),
						  WS_OVERLAPPEDWINDOW | WS_MAXIMIZE | WS_CLIPSIBLINGS,
						  POS_X, POS_Y, 
						  SIZE_X, SIZE_Y,
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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
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