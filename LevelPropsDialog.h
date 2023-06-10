#ifndef LEVELPROPSDIALOG_H
#define LEVELPROPSDIALOG_H

#pragma warning(disable : 4786)

#include <string>
#include <list>
#include <windows.h>
#include "Map.h"

struct LEVELPROPSPARAMS
{
	BOOL CreateNewLevel, LevelSizeChanged;
	Duke2Map* Level;
	std::list<std::string>* CZoneList, *BackdropList, *MusicList;
};

BOOL CALLBACK LevelPropsDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

#endif