#ifndef MAPOVERVIEW_H
#define MAPOVERVIEW_H

#include <windows.h>

const RGBQUAD TILECOLOR_NULL = {0x66, 0x66, 0x66, 0};
const RGBQUAD TILECOLOR_BG = {0x88, 0x88, 0x88, 0};
const RGBQUAD TILECOLOR_CLIMB = {0x00, 0xAA, 0xAA};
const RGBQUAD TILECOLOR_SOLID = {0xDD, 0xDD, 0xDD, 0};

BOOL CALLBACK
  MapOverviewDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

#endif