#ifndef MESSAGES_H
#define MESSAGES_H

#include <windows.h>

LRESULT CALLBACK
  MainWindowProc(HWND hWndLocal, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK
  DAWindowProc(HWND hWndLocal, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK
  TSWindowProc(HWND hWndLocal, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

void InitMessages(void);
void CleanUpMessages(void);

#endif