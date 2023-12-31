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


#pragma warning(disable : 4786)

#include "Shared.h"
#include "Log.h"
#include <list>
#include <string>
#include <stdio.h>

using namespace std;

list<string> ArgList;

int Error(char* msg, ...)
{
#ifdef _DEBUG
  static BYTE NrErrors = 0;

  if (++NrErrors > MAX_ERRORS)
  {
    WriteLogFile(
      "Error(): To many errors occured (%u) - terminating program\n", NrErrors);
    MessageBox(
      hWnd,
      "Error(): To many errors occured - terminating program",
      "Error",
      MB_OK | MB_ICONERROR);
    ExitProcess(-1);
    return 0;
  }
#endif

  va_list ap;
  va_start(ap, msg);

  char* Buffer = new char[256];

  vsprintf(Buffer, msg, ap);

  WriteLogFile("ERROR: %s\n", Buffer);
  MessageBox(hWnd, Buffer, "Error", MB_OK | MB_ICONERROR);

  delete[] Buffer;

  va_end(ap);

  return 0;
}

int Message(char* msg, UINT Flags, ...)
{
  va_list ap;
  va_start(ap, msg);

  char* Buffer = new char[256];

  vsprintf(Buffer, msg, ap);

  int RetVal = MessageBox(hWnd, Buffer, APPNAME, Flags);

  delete[] Buffer;

  va_end(ap);

  return RetVal;
}


void ProcessCmdLine(void)
{
  int Length = 0;

  BOOL InQuotes = FALSE;

  char* pCmdLine = GetCommandLine();

  char* pCmdLineIn = new char[lstrlen(pCmdLine) + 1];
  strcpy(pCmdLineIn, pCmdLine);

  while (*pCmdLineIn == ' ')
    pCmdLineIn++;

  char* pCmdLineStart = pCmdLineIn;
  char* pCmdLinePartToCopy = pCmdLineIn;

  while (*pCmdLineIn)
  {
    switch (*pCmdLineIn++)
    {
      case ' ':
        if (!InQuotes)
        {
          char* Arg = new char[Length + 1];
          ZeroMemory(Arg, Length + 1);
          strncpy(Arg, pCmdLinePartToCopy, Length);
          string ArgS = Arg;
          ArgList.push_back(ArgS);
          delete Arg;

          Length = -1;

          while (*pCmdLineIn == ' ')
            pCmdLineIn++;

          pCmdLinePartToCopy = pCmdLineIn;
        }
        break;

      case '"':
        if (!InQuotes)
        {
          InQuotes = TRUE;
          Length = -1;
          pCmdLinePartToCopy++;

          while (*pCmdLineIn == ' ')
          {
            pCmdLineIn++;
            pCmdLinePartToCopy++;
          }

          if (*pCmdLineIn == '"')
          {
            InQuotes = FALSE;

            while (*pCmdLineIn == ' ')
              pCmdLineIn++;

            pCmdLinePartToCopy = pCmdLineIn;
          }
        }
        else
        {
          InQuotes = FALSE;

          char* Arg = new char[Length + 1];
          ZeroMemory(Arg, Length + 1);
          strncpy(Arg, pCmdLinePartToCopy, Length);
          string ArgS = Arg;
          ArgList.push_back(ArgS);
          delete Arg;


          Length = -1;

          while (*pCmdLineIn == ' ')
            pCmdLineIn++;

          pCmdLinePartToCopy = pCmdLineIn;
        }
        break;
    }

    Length++;
  }

  char* Arg = new char[Length + 1];
  ZeroMemory(Arg, Length + 1);
  strncpy(Arg, pCmdLinePartToCopy, Length);
  string ArgS = Arg;
  ArgList.push_back(ArgS);
  delete Arg;


  delete[] pCmdLineStart;
}

BOOL GetArg(UINT Nr, char* Dst, UINT* Len)
{
  list<string>::iterator it = ArgList.begin();

  if (Nr >= ArgList.size())
    return Error(
      "GetArg(): Nr of argument requested (%u) is larger than the number of existing args!",
      Nr);

  for (UINT i = 0; i < Nr; i++)
  {
    it++;
  }

  if (!Dst)
  {
    if (Len)
    {
      *Len = (*it).size();
      return TRUE;
    }

    return Error("GetArg(): Neither Len nor Dst are valid pointers!");
  }

  strcpy(Dst, (*it).c_str());

  return TRUE;
}
