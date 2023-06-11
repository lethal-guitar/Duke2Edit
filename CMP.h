#ifndef CMP_H
#define CMP_H

#pragma warning(disable : 4786)

#include <map>
#include <string>
#include <windows.h>

using namespace std;

struct CMP_DirectoryEntry
{
  char FileName[12];
  DWORD FileOffset;
  DWORD FileSize;
};

enum FILETYPE
{
  TYPE_IMF,
  TYPE_PAL,
  TYPE_CZONE,
  TYPE_FULLSCREENIMG,
  TYPE_BGDROP,
  TYPE_DOSTEXT,
  TYPE_VOC,
  TYPE_LEVEL,
  TYPE_ASCIITEXT,
  TYPE_UNKNOWN
};

class CMP_File
{
private:
  map<string, CMP_DirectoryEntry*> Directory;
  HANDLE hFile;

  void ClearDirectory(void);
  BOOL CheckIfAscii(char* pTest, int Bytes);
  BOOL CheckIfLevel(char* pTest);

  void StrToLower(char* Str);
  void StrToLower(string& Str);

public:
  ~CMP_File() { Close(); }

  BOOL Open(string FileName);
  void Close(void)
  {
    ClearDirectory();
    CloseHandle(hFile);
    hFile = NULL;
  }
  int GetNrFiles(void) { return Directory.size(); }
  void GetFileNames(string Dst[]);
  int GetFileSize(string Name);
  BOOL GetFile(string Name, BYTE Dst[], int Bytes = 0, int Offset = 0);
  FILETYPE DetermineFileType(string Name);
};

#endif