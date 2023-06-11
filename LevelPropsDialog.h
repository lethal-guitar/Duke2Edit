#ifndef LEVELPROPSDIALOG_H
#define LEVELPROPSDIALOG_H

#pragma warning(disable : 4786)

#include "Map.h"
#include <list>
#include <string>
#include <windows.h>

struct LEVELPROPSPARAMS
{
  BOOL CreateNewLevel, LevelSizeChanged;
  Duke2Map* Level;
  std::list<std::string>*CZoneList, *BackdropList, *MusicList;
};

BOOL CALLBACK
  LevelPropsDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

#endif