#ifndef TILESELECTION_H
#define TILESELECTION_H

#include <windows.h>

void OnTSMouseMove(HWND hWndMM, WPARAM wParam, LPARAM lParam);
void OnTSLButtonUp(HWND hWndLocal, WPARAM wParam, LPARAM lParam);
void OnTSLButtonDown(HWND hWndLocal, WPARAM wParam, LPARAM lParam);
void EndTSFocusDragging(void);

#endif