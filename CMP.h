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

// This class implements the game's group file format.
// See https://moddingwiki.shikadi.net/wiki/CMP_Format.
//
// For a much cleaner and more modern implementation, see:
// https://github.com/lethal-guitar/RigelEngine/blob/4d97e1e3582cdf6caa78e7d73b8865885793529d/src/assets/cmp_file_package.hpp#L32
class CMP_File
{
private:
  // [REVIEW NOTE] It would be much better to hold the map entries by value, not
  // by pointer. That would avoid a lot of manual memory management.
  map<string, CMP_DirectoryEntry*> Directory;

  // [REVIEW NOTE] For some reason, this uses the Win32 file APIs instead of
  // their standard library counterparts. Using a std::ifstream instead would
  // simplify the code, make it platform-independent, and avoid the need for a
  // destructor.
  HANDLE hFile;

  void ClearDirectory(void);
  BOOL CheckIfAscii(char* pTest, int Bytes);
  BOOL CheckIfLevel(char* pTest);

  void StrToLower(char* Str);
  void StrToLower(string& Str);

public:
  ~CMP_File() { Close(); }

  // [REVIEW NOTE] Ideally, this should be the constructor to reduce the amount
  // of state.
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