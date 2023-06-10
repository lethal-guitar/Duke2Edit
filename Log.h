#ifndef LOG_H
#define LOG_H

#include <windows.h>

extern BOOL OpenLogFile(char*);
extern void CloseLogFile(void);
extern void WriteLogFile(char*, ...);

#endif