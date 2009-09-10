/*
$Date$
$Rev$
*/

#ifndef WSCONFIG_H_
#define WSCONFIG_H_

#include <stdio.h>

#define FNAME_LENGTH 512

extern char* Command;
extern char* AppDirectory;
extern char IniName[FNAME_LENGTH+1];

extern char* DefaultDir;
extern char* Title;
extern char CartName[FNAME_LENGTH+1];

#define LK_TMP 0
#define LK_SIO 1
#define LK_SOUND 2
#define LK_SPRITE 3
extern FILE *Log;
extern int LogKind;

extern int Verbose;

void ConfigCreate(void);
void ConfigRelease(void);
void LogFile(int Kind, char *string);

#endif
