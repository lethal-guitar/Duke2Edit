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


#include "Log.h"
#include <stdio.h>

static BOOL Logging;

BOOL OpenLogFile(char* AppName)
{
  Logging = TRUE;

  FILE* fpLogFile = NULL;

  if (NULL == (fpLogFile = fopen("Log.txt", "w")))
    return FALSE;

  if (AppName && AppName[0])
    fprintf(fpLogFile, "%s Logfile\n", AppName);

  char* DateStr = new char[GetDateFormat(NULL, 0, NULL, NULL, NULL, 0)];

  GetDateFormat(NULL, 0, NULL, NULL, DateStr, lstrlen(DateStr));

  char* TimeStr = new char[GetTimeFormat(NULL, 0, NULL, NULL, NULL, 0)];

  GetTimeFormat(NULL, 0, NULL, NULL, TimeStr, lstrlen(TimeStr));

  fprintf(fpLogFile, "Logfile opened %s at %s\n", DateStr, TimeStr);

  delete[] DateStr;
  delete[] TimeStr;

  fprintf(fpLogFile, "----------------------------------------------\n");
  fclose(fpLogFile);

  return TRUE;
}

void CloseLogFile(void)
{
  Logging = FALSE;

  FILE* fpLogFile = fopen("Log.txt", "a");

  fprintf(fpLogFile, "----------------------------------------------\n");

  char* DateStr = new char[GetDateFormat(NULL, 0, NULL, NULL, NULL, 0)];

  GetDateFormat(NULL, 0, NULL, NULL, DateStr, lstrlen(DateStr));

  char* TimeStr = new char[GetTimeFormat(NULL, 0, NULL, NULL, NULL, 0)];

  GetTimeFormat(NULL, 0, NULL, NULL, TimeStr, lstrlen(TimeStr));

  fprintf(fpLogFile, "Logfile closed %s at %s\n", DateStr, TimeStr);

  delete[] DateStr;
  delete[] TimeStr;

  fclose(fpLogFile);

  fpLogFile = NULL;
}

void WriteLogFile(char* Str, ...)
{
  if (Logging)
  {
    va_list ap;
    va_start(ap, Str);

    FILE* fpLogFile = fopen("Log.txt", "a");
    vfprintf(fpLogFile, Str, ap);
    fclose(fpLogFile);

    va_end(ap);
  }
}