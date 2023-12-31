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


#include "CMP.h"
#include <algorithm>

void CMP_File::StrToLower(char* Str)
{
  for (int i = 0; i < strlen(Str); i++)
  {
    Str[i] = tolower(Str[i]);
  }
}

void CMP_File::StrToLower(string& Str)
{
  for (int i = 0; i < Str.length(); i++)
  {
    Str[i] = tolower(Str[i]);
  }
}

void CMP_File::ClearDirectory(void)
{
  for (map<string, CMP_DirectoryEntry*>::iterator it = Directory.begin();
       it != Directory.end();
       it++)
  {
    // delete[] (*it).first;
    delete (*it).second;
  }

  Directory.clear();
}

BOOL CMP_File::Open(string FileName)
{
  Close();

  hFile =
    CreateFile(FileName.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);

  if (!hFile)
    return FALSE;

  while (1)
  {
    CMP_DirectoryEntry* pNewEntry = new CMP_DirectoryEntry;
    DWORD NrRead = 0;

    BOOL Result =
      ReadFile(hFile, pNewEntry, sizeof(CMP_DirectoryEntry), &NrRead, NULL);

    if (!Result || NrRead != sizeof(CMP_DirectoryEntry))
    {
      ClearDirectory();
      return FALSE;
    }

    if (!pNewEntry->FileOffset && !pNewEntry->FileSize)
      break;

    char* Name = new char[13];
    ZeroMemory(Name, 13);
    strncpy(Name, pNewEntry->FileName, 12);
    StrToLower(Name);

    Directory[Name] = pNewEntry;

    delete[] Name;
  }

  return TRUE;
}

void CMP_File::GetFileNames(string Dst[])
{
  map<string, CMP_DirectoryEntry*>::iterator it = Directory.begin();

  for (int i = 0; i < Directory.size(); i++)
  {
    CMP_DirectoryEntry* pEntry = (*it).second;
    char* Name = new char[13];
    ZeroMemory(Name, 13);
    strncpy(Name, pEntry->FileName, 12);
    Dst[i] = Name;
    delete[] Name;

    it++;
  }
}

int CMP_File::GetFileSize(string Name)
{
  StrToLower(Name);
  map<string, CMP_DirectoryEntry*>::iterator it = Directory.find(Name);

  if (it != Directory.end())
    return (*it).second->FileSize;

  return 0;
}

BOOL CMP_File::GetFile(string Name, BYTE Dst[], int Bytes, int Offset)
{
  StrToLower(Name);
  map<string, CMP_DirectoryEntry*>::iterator it = Directory.find(Name);

  if (it == Directory.end())
    return FALSE;

  DWORD NrRead = 0;

  SetFilePointer(hFile, (*it).second->FileOffset + Offset, NULL, FILE_BEGIN);
  BOOL Result = ReadFile(
    hFile, Dst, (Bytes) ? Bytes : (*it).second->FileSize, &NrRead, NULL);

  if (!Result || NrRead != ((Bytes) ? Bytes : (*it).second->FileSize - Offset))
    return FALSE;

  return TRUE;
}

BOOL CMP_File::CheckIfLevel(char* pTest)
{
  char* pStart = pTest + 2;

  if (!strncmp(pStart, "czone", 5))
    return TRUE;

  return FALSE;
}

BOOL CMP_File::CheckIfAscii(char* pTest, int Bytes)
{
  int NrNotAscii = 0;

  for (int i = 0; i < Bytes; i++)
  {
    if (!isalnum((unsigned char)pTest[i]))
    {
      if (++NrNotAscii >= 4)
      {
        return FALSE;
      }
    }
  }

  return TRUE;
}

FILETYPE CMP_File::DetermineFileType(string Name)
{
  StrToLower(Name);

  if (Name.substr(Name.find('.') + 1, 3) == "imf")
  {
    // TypeStr = "IMF music file";
    return TYPE_IMF;
  }
  else if (Name.substr(Name.find('.') + 1, 3) == "pal")
  {
    // TypeStr = "EGA 6bit 16-color palette";
    return TYPE_PAL;
  }
  else
  {
    switch (GetFileSize(Name))
    {
      case 42000:
        // TypeStr = "CZone tileset";
        return TYPE_CZONE;
        break;

      case 32048:
        // TypeStr = "Fullscreen image";
        return TYPE_FULLSCREENIMG;
        break;

      case 32000:
        // TypeStr = "Backdrop image";
        return TYPE_BGDROP;
        break;

      case 4000:
        // TypeStr = "DOS text screen";
        return TYPE_DOSTEXT;
        break;

      default:
        FILETYPE Ret;

        BYTE* FileData = new BYTE[19];

        GetFile(Name, FileData, 19);

        if (!strncmp((char*)FileData, "Creative Voice File", 19))
        {
          // TypeStr = "Creative Voice File (VOC Format)";
          Ret = TYPE_VOC;
        }
        else if (CheckIfLevel((char*)FileData))
        {
          // TypeStr = "Level file";
          Ret = TYPE_LEVEL;
        }
        else if (CheckIfAscii((char*)FileData, 5))
        {
          // TypeStr = "ASCII text file";
          Ret = TYPE_ASCIITEXT;
        }
        else
        {
          // TypeStr = "Unknown";
          Ret = TYPE_UNKNOWN;
        }

        delete[] FileData;
        return Ret;
        break;
    }
  }
}
