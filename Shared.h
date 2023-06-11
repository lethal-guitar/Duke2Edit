#ifndef SHARED_H
#define SHARED_H

#include <windows.h>
//#include "Gui.h"

#ifdef _DEBUG
const int MAX_ERRORS = 10;
#endif

const char APPNAME[] = "Duke Nukem II Level Editor";

extern HINSTANCE hInstance;
extern HWND hWnd;
// extern GuiClass  Gui;

int Error(char*, ...);
int Message(char*, UINT = MB_OK, ...);
BOOL GetArg(UINT Nr, char* Dst, UINT* Len);
void ProcessCmdLine(void);
void SizeStatusbarParts(void);

#endif