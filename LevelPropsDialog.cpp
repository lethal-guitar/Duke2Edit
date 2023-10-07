#include "LevelPropsDialog.h"
#include "Duke2Files.h"
#include "EditorGFX.h"
#include "Gui.h"
#include "SelectCopyPaste.h"
#include "Shared.h"
#include "resource.h"

static HBITMAP hBmCZone, hBmBackdrop;
static LEVELPROPSPARAMS* Params;

static void OnInitDialog(HWND hDlg, WPARAM wParam, LPARAM lParam);
static void OnCommand(HWND hDlg, WPARAM wParam, LPARAM lParam);

BOOL CALLBACK
  LevelPropsDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg)
  {
    case WM_INITDIALOG:
      OnInitDialog(hDlg, wParam, lParam);
      return TRUE;
      break;

    case WM_COMMAND:
      OnCommand(hDlg, wParam, lParam);
      break;

    case WM_DESTROY:
      DeleteObject(hBmCZone);
      DeleteObject(hBmBackdrop);
      break;

    default:
      return FALSE;
  }

  return TRUE;
}

void EnableBGChange(HWND hDlg, BOOL Enable)
{
  if (!IsDlgButtonChecked(hDlg, IDC_CHECKCHANGEBG) && Enable)
    return;

  EnableWindow(GetDlgItem(hDlg, IDC_RADIOCHGBG1), Enable);
  EnableWindow(GetDlgItem(hDlg, IDC_RADIOCHGBG2), Enable);
  EnableWindow(GetDlgItem(hDlg, IDC_TEXTCHGBG), Enable);
  EnableWindow(GetDlgItem(hDlg, IDC_COMBOCHGBG), Enable);
  EnableWindow(GetDlgItem(hDlg, IDC_CHECKPREVIEWCHGBG), Enable);

  if (IsDlgButtonChecked(hDlg, IDC_CHECKPREVIEWCHGBG))
  {
    DeleteObject(hBmBackdrop);
    char FileName[13];

    if (Enable)
    {
      GetDlgItemText(hDlg, IDC_COMBOCHGBG, FileName, 13);
      LoadImageFromCMP(FileName, &hBmBackdrop);
      SendDlgItemMessage(
        hDlg,
        IDC_BMPPREVIEWBDROP,
        STM_SETIMAGE,
        WPARAM(IMAGE_BITMAP),
        LPARAM(hBmBackdrop));
    }
    else
    {
      GetDlgItemText(hDlg, IDC_COMBOBDROP, FileName, 13);
      LoadImageFromCMP(FileName, &hBmBackdrop);
      SendDlgItemMessage(
        hDlg,
        IDC_BMPPREVIEWBDROP,
        STM_SETIMAGE,
        WPARAM(IMAGE_BITMAP),
        LPARAM(hBmBackdrop));
    }
  }
}

void OnInitDialog(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
  Params = ((LEVELPROPSPARAMS*)lParam);

  if (Params->CreateNewLevel)
    SetWindowText(hDlg, "Set properties for new level");
  else
    SetWindowText(hDlg, "Change level properties");

  int i = 0;

  list<string>::iterator it;
  for (it = Params->CZoneList->begin();
       it != Params->CZoneList->end();
       it++, i++)
  {
    SendDlgItemMessage(
      hDlg, IDC_COMBOCZONE, CB_ADDSTRING, 0, LPARAM((*it).c_str()));

    if (
      !Params->CreateNewLevel &&
      !_stricmp((*it).c_str(), Params->Level->GetCZone()))
      SendDlgItemMessage(hDlg, IDC_COMBOCZONE, CB_SETCURSEL, i, 0);
    else if (Params->CreateNewLevel && !_stricmp((*it).c_str(), START_CZONE))
      SendDlgItemMessage(hDlg, IDC_COMBOCZONE, CB_SETCURSEL, i, 0);
  }

  i = 0;

  for (it = Params->BackdropList->begin(); it != Params->BackdropList->end();
       it++, i++)
  {
    SendDlgItemMessage(
      hDlg, IDC_COMBOBDROP, CB_ADDSTRING, 0, LPARAM((*it).c_str()));

    if (
      !Params->CreateNewLevel &&
      !_stricmp((*it).c_str(), Params->Level->GetBGDrop()))
      SendDlgItemMessage(hDlg, IDC_COMBOBDROP, CB_SETCURSEL, i, 0);
    else if (Params->CreateNewLevel && !_stricmp((*it).c_str(), START_BGDROP))
      SendDlgItemMessage(hDlg, IDC_COMBOBDROP, CB_SETCURSEL, i, 0);
  }

  i = 0;

  for (it = Params->MusicList->begin(); it != Params->MusicList->end();
       it++, i++)
  {
    SendDlgItemMessage(
      hDlg, IDC_COMBOMUSIC, CB_ADDSTRING, 0, LPARAM((*it).c_str()));

    if (
      !Params->CreateNewLevel &&
      !_stricmp((*it).c_str(), Params->Level->GetMusic()))
      SendDlgItemMessage(hDlg, IDC_COMBOMUSIC, CB_SETCURSEL, i, 0);
    else if (Params->CreateNewLevel && !_stricmp((*it).c_str(), START_MUSIC))
      SendDlgItemMessage(hDlg, IDC_COMBOMUSIC, CB_SETCURSEL, i, 0);
  }

  char FileName[13];

  GetDlgItemText(hDlg, IDC_COMBOCZONE, FileName, 13);

  LoadImageFromCMP(FileName, &hBmCZone);

  SendDlgItemMessage(
    hDlg,
    IDC_BMPPREVIEWCZONE,
    STM_SETIMAGE,
    WPARAM(IMAGE_BITMAP),
    LPARAM(hBmCZone));

  GetDlgItemText(hDlg, IDC_COMBOBDROP, FileName, 13);

  LoadImageFromCMP(FileName, &hBmBackdrop);

  SendDlgItemMessage(
    hDlg,
    IDC_BMPPREVIEWBDROP,
    STM_SETIMAGE,
    WPARAM(IMAGE_BITMAP),
    LPARAM(hBmBackdrop));

  WORD Flags = START_FLAGS;

  if (!Params->CreateNewLevel)
    Flags = Params->Level->GetFlags();


  if (
    Flags &
    (LF_SCROLLHORZVERT | LF_SCROLLHORZ | LF_AUTOSCROLLHORZ | LF_AUTOSCROLLVERT))
  {
    CheckDlgButton(hDlg, IDC_CHECKBGSCROLL, BST_CHECKED);
    EnableWindow(GetDlgItem(hDlg, IDC_RADIOSCROLL1), TRUE);
    EnableWindow(GetDlgItem(hDlg, IDC_RADIOSCROLL2), TRUE);
    EnableWindow(GetDlgItem(hDlg, IDC_RADIOSCROLL3), TRUE);
    EnableWindow(GetDlgItem(hDlg, IDC_RADIOSCROLL4), TRUE);
  }
  else
  {
    CheckDlgButton(hDlg, IDC_CHECKBGSCROLL, BST_UNCHECKED);
    EnableWindow(GetDlgItem(hDlg, IDC_RADIOSCROLL1), FALSE);
    EnableWindow(GetDlgItem(hDlg, IDC_RADIOSCROLL2), FALSE);
    EnableWindow(GetDlgItem(hDlg, IDC_RADIOSCROLL3), FALSE);
    EnableWindow(GetDlgItem(hDlg, IDC_RADIOSCROLL4), FALSE);
  }

  if (
    Flags & LF_SCROLLHORZVERT ||
    !(Flags & LF_SCROLLHORZVERT) && !(Flags & LF_SCROLLHORZ) &&
      !(Flags & LF_AUTOSCROLLHORZ) && !(Flags & LF_AUTOSCROLLVERT))
    CheckDlgButton(hDlg, IDC_RADIOSCROLL1, BST_CHECKED);

  if (Flags & LF_SCROLLHORZ)
    CheckDlgButton(hDlg, IDC_RADIOSCROLL2, BST_CHECKED);

  if (Flags & LF_AUTOSCROLLHORZ)
    CheckDlgButton(hDlg, IDC_RADIOSCROLL3, BST_CHECKED);

  if (Flags & LF_AUTOSCROLLVERT)
    CheckDlgButton(hDlg, IDC_RADIOSCROLL4, BST_CHECKED);

  if (Flags & LF_EARTHQUAKE)
    CheckDlgButton(hDlg, IDC_CHECKEARTHQUAKE, BST_CHECKED);
  else
    CheckDlgButton(hDlg, IDC_CHECKEARTHQUAKE, BST_UNCHECKED);

  i = 0;

  for (it = Params->BackdropList->begin(); it != Params->BackdropList->end();
       it++, i++)
  {
    SendDlgItemMessage(
      hDlg, IDC_COMBOCHGBG, CB_ADDSTRING, 0, LPARAM((*it).c_str()));

    if (
      !Params->CreateNewLevel && Flags & (LF_CHANGEBG1 | LF_CHANGEBG2) &&
      atoi((*it).substr(4, 2).c_str()) == HIBYTE(Flags))
      SendDlgItemMessage(hDlg, IDC_COMBOCHGBG, CB_SETCURSEL, i, 0);
  }

  if (
    Flags & (LF_CHANGEBG1 | LF_CHANGEBG2) && !(Flags & LF_AUTOSCROLLHORZ) &&
    !(Flags & LF_AUTOSCROLLVERT))
  {
    CheckDlgButton(hDlg, IDC_CHECKCHANGEBG, BST_CHECKED);
    EnableBGChange(hDlg, TRUE);
  }
  else
  {
    CheckDlgButton(hDlg, IDC_CHECKCHANGEBG, BST_UNCHECKED);
    EnableBGChange(hDlg, FALSE);
    SendDlgItemMessage(hDlg, IDC_COMBOCHGBG, CB_SETCURSEL, 0, 0);
  }

  if (Flags & LF_AUTOSCROLLHORZ || Flags & LF_AUTOSCROLLVERT)
    EnableWindow(GetDlgItem(hDlg, IDC_CHECKCHANGEBG), FALSE);
  else
    EnableWindow(GetDlgItem(hDlg, IDC_CHECKCHANGEBG), TRUE);

  if (
    Flags & LF_CHANGEBG1 || !(Flags & LF_CHANGEBG1) && !(Flags & LF_CHANGEBG2))
    CheckDlgButton(hDlg, IDC_RADIOCHGBG1, BST_CHECKED);

  if (Flags & LF_CHANGEBG2)
    CheckDlgButton(hDlg, IDC_RADIOCHGBG2, BST_CHECKED);

  CheckDlgButton(hDlg, IDC_CHECKPREVIEWCHGBG, BST_UNCHECKED);

  for (i = 0; VALID_LEVEL_WIDTHS[i]; i++)
  {
    char Str[8];
    itoa(VALID_LEVEL_WIDTHS[i], Str, 10);
    SendDlgItemMessage(hDlg, IDC_COMBOLEVELWIDTH, CB_ADDSTRING, 0, LPARAM(Str));

    if (
      !Params->CreateNewLevel &&
      VALID_LEVEL_WIDTHS[i] == Params->Level->GetLevelWidth())
    {
      SendDlgItemMessage(hDlg, IDC_COMBOLEVELWIDTH, CB_SETCURSEL, i, 0);
      SetDlgItemInt(
        hDlg,
        IDC_TEXTLVLHEIGHT,
        LEVEL_DATA_WORDS / VALID_LEVEL_WIDTHS[i],
        TRUE);
    }
    else if (
      Params->CreateNewLevel && VALID_LEVEL_WIDTHS[i] == START_LEVELWIDTH)
    {
      SendDlgItemMessage(hDlg, IDC_COMBOLEVELWIDTH, CB_SETCURSEL, i, 0);
      SetDlgItemInt(
        hDlg,
        IDC_TEXTLVLHEIGHT,
        LEVEL_DATA_WORDS / VALID_LEVEL_WIDTHS[i],
        TRUE);
    }
  }
}

WORD ReadFlagsFromDialog(HWND hDlg)
{
  char Name[13];
  WORD Flags = 0;

  if (IsDlgButtonChecked(hDlg, IDC_CHECKBGSCROLL))
  {
    if (IsDlgButtonChecked(hDlg, IDC_RADIOSCROLL1))
      Flags |= LF_SCROLLHORZVERT;
    else if (IsDlgButtonChecked(hDlg, IDC_RADIOSCROLL2))
      Flags |= LF_SCROLLHORZ;
    else if (IsDlgButtonChecked(hDlg, IDC_RADIOSCROLL3))
      Flags |= LF_AUTOSCROLLHORZ;
    else if (IsDlgButtonChecked(hDlg, IDC_RADIOSCROLL4))
      Flags |= LF_AUTOSCROLLVERT;
  }

  if (IsDlgButtonChecked(hDlg, IDC_CHECKEARTHQUAKE))
    Flags |= LF_EARTHQUAKE;

  if (IsDlgButtonChecked(hDlg, IDC_CHECKCHANGEBG))
  {
    if (IsDlgButtonChecked(hDlg, IDC_RADIOCHGBG1))
      Flags |= LF_CHANGEBG1;
    else if (IsDlgButtonChecked(hDlg, IDC_RADIOCHGBG2))
      Flags |= LF_CHANGEBG2;

    GetDlgItemText(hDlg, IDC_COMBOCHGBG, Name, 13);

    string NameS = Name;

    Flags |= atoi(NameS.substr(4, 2).c_str()) << 8;
  }

  return Flags;
}

void OnCommand(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
  char FileName[13];

  switch (LOWORD(wParam))
  {
    case IDCANCEL:
      Params->LevelSizeChanged = FALSE;
      EndDialog(hDlg, FALSE);
      break;

    case IDOK:
      Params->LevelSizeChanged = FALSE;

      int w = GetDlgItemInt(hDlg, IDC_COMBOLEVELWIDTH, NULL, FALSE);

      if (w != Params->Level->GetLevelWidth())
      {
        if (
          IDYES ==
          Message(
            "Warning: Changing the size of the level will truncate a part of your level! Proceed?",
            MB_YESNO | MB_ICONQUESTION))
        {
          Params->Level->ChangeWidth(w);
          Params->LevelSizeChanged = TRUE; // LevelSizeChange();
        }
        else
        {
          for (int i = 0; VALID_LEVEL_WIDTHS[i]; i++)
          {
            if (VALID_LEVEL_WIDTHS[i] == Params->Level->GetLevelWidth())
            {
              SendDlgItemMessage(hDlg, IDC_COMBOLEVELWIDTH, CB_SETCURSEL, i, 0);
              SetDlgItemInt(
                hDlg,
                IDC_TEXTLVLHEIGHT,
                LEVEL_DATA_WORDS / VALID_LEVEL_WIDTHS[i],
                TRUE);
            }
          }

          return;
        }
      }

      char CZone[13], BDrop[13], Music[13];

      GetDlgItemText(hDlg, IDC_COMBOCZONE, CZone, 13);
      GetDlgItemText(hDlg, IDC_COMBOBDROP, BDrop, 13);
      GetDlgItemText(hDlg, IDC_COMBOMUSIC, Music, 13);

      if (!Params->CreateNewLevel)
      {
        SelectCopyPaste.Deselect(&Gui.GetScrollPos(), Params->Level);
        Params->Level->SetCZone(CZone);
        Params->Level->SetBGDrop(BDrop);
        Params->Level->SetMusic(Music);
        Params->Level->SetFlags(ReadFlagsFromDialog(hDlg));
      }
      else
      {
        SelectCopyPaste.Deselect(&Gui.GetScrollPos(), Params->Level);
        Params->Level->Create(
          w, CZone, Music, BDrop, ReadFlagsFromDialog(hDlg));
      }

      EndDialog(hDlg, TRUE);
      break;
  }

  switch (HIWORD(wParam))
  {
    case CBN_SELCHANGE:
      switch (LOWORD(wParam))
      {
        case IDC_COMBOCZONE:
          GetDlgItemText(hDlg, IDC_COMBOCZONE, FileName, 13);
          DeleteObject(hBmCZone);
          LoadImageFromCMP(FileName, &hBmCZone);
          SendDlgItemMessage(
            hDlg,
            IDC_BMPPREVIEWCZONE,
            STM_SETIMAGE,
            WPARAM(IMAGE_BITMAP),
            LPARAM(hBmCZone));
          break;

        case IDC_COMBOBDROP:
          GetDlgItemText(hDlg, IDC_COMBOBDROP, FileName, 13);
          DeleteObject(hBmBackdrop);
          LoadImageFromCMP(FileName, &hBmBackdrop);
          SendDlgItemMessage(
            hDlg,
            IDC_BMPPREVIEWBDROP,
            STM_SETIMAGE,
            WPARAM(IMAGE_BITMAP),
            LPARAM(hBmBackdrop));
          break;

        case IDC_COMBOCHGBG:
          if (!IsDlgButtonChecked(hDlg, IDC_CHECKPREVIEWCHGBG))
            break;

          GetDlgItemText(hDlg, IDC_COMBOCHGBG, FileName, 13);
          DeleteObject(hBmBackdrop);
          LoadImageFromCMP(FileName, &hBmBackdrop);
          SendDlgItemMessage(
            hDlg,
            IDC_BMPPREVIEWBDROP,
            STM_SETIMAGE,
            WPARAM(IMAGE_BITMAP),
            LPARAM(hBmBackdrop));
          break;

        case IDC_COMBOLEVELWIDTH:
          SetDlgItemInt(
            hDlg,
            IDC_TEXTLVLHEIGHT,
            LEVEL_DATA_WORDS /
              VALID_LEVEL_WIDTHS[SendDlgItemMessage(
                hDlg, IDC_COMBOLEVELWIDTH, CB_GETCURSEL, 0, 0)],
            TRUE);
          break;
      }
      break;

    case BN_CLICKED:
      switch (LOWORD(wParam))
      {
        case IDC_CHECKBGSCROLL:
          if (IsDlgButtonChecked(hDlg, IDC_CHECKBGSCROLL))
          {
            EnableWindow(GetDlgItem(hDlg, IDC_RADIOSCROLL1), TRUE);
            EnableWindow(GetDlgItem(hDlg, IDC_RADIOSCROLL2), TRUE);
            EnableWindow(GetDlgItem(hDlg, IDC_RADIOSCROLL3), TRUE);
            EnableWindow(GetDlgItem(hDlg, IDC_RADIOSCROLL4), TRUE);

            if (
              IsDlgButtonChecked(hDlg, IDC_RADIOSCROLL3) ||
              IsDlgButtonChecked(hDlg, IDC_RADIOSCROLL4))
            {
              EnableWindow(GetDlgItem(hDlg, IDC_CHECKCHANGEBG), FALSE);
              EnableBGChange(hDlg, FALSE);
            }
          }
          else
          {
            EnableWindow(GetDlgItem(hDlg, IDC_RADIOSCROLL1), FALSE);
            EnableWindow(GetDlgItem(hDlg, IDC_RADIOSCROLL2), FALSE);
            EnableWindow(GetDlgItem(hDlg, IDC_RADIOSCROLL3), FALSE);
            EnableWindow(GetDlgItem(hDlg, IDC_RADIOSCROLL4), FALSE);
            EnableWindow(GetDlgItem(hDlg, IDC_CHECKCHANGEBG), TRUE);
            EnableBGChange(hDlg, TRUE);
          }
          break;

        case IDC_CHECKCHANGEBG:
          if (IsDlgButtonChecked(hDlg, IDC_CHECKCHANGEBG))
            EnableBGChange(hDlg, TRUE);
          else
            EnableBGChange(hDlg, FALSE);
          break;

        case IDC_RADIOSCROLL1:
        case IDC_RADIOSCROLL2:
          EnableWindow(GetDlgItem(hDlg, IDC_CHECKCHANGEBG), TRUE);
          EnableBGChange(hDlg, TRUE);
          break;

        case IDC_RADIOSCROLL3:
        case IDC_RADIOSCROLL4:
          EnableWindow(GetDlgItem(hDlg, IDC_CHECKCHANGEBG), FALSE);
          EnableBGChange(hDlg, FALSE);
          break;

        case IDC_CHECKPREVIEWCHGBG:
          DeleteObject(hBmBackdrop);

          if (IsDlgButtonChecked(hDlg, IDC_CHECKPREVIEWCHGBG))
          {
            GetDlgItemText(hDlg, IDC_COMBOCHGBG, FileName, 13);
            LoadImageFromCMP(FileName, &hBmBackdrop);
            SendDlgItemMessage(
              hDlg,
              IDC_BMPPREVIEWBDROP,
              STM_SETIMAGE,
              WPARAM(IMAGE_BITMAP),
              LPARAM(hBmBackdrop));
          }
          else
          {
            GetDlgItemText(hDlg, IDC_COMBOBDROP, FileName, 13);
            LoadImageFromCMP(FileName, &hBmBackdrop);
            SendDlgItemMessage(
              hDlg,
              IDC_BMPPREVIEWBDROP,
              STM_SETIMAGE,
              WPARAM(IMAGE_BITMAP),
              LPARAM(hBmBackdrop));
          }
          break;
      }
      break;
  }
}