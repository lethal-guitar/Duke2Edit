#include <stdio.h>
#include "Log.h"

static BOOL Logging;

BOOL OpenLogFile(char* AppName)
{
	Logging = TRUE;

	FILE* fpLogFile = NULL;
	
	if (NULL == (fpLogFile = fopen("Log.txt", "w"))) return FALSE;

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