﻿/*
$Date$
$Rev$
*/

#ifndef WS_H_
#define WS_H_

#include "WSHard.h"

struct EEPROM
{
    WORD *data;
    int we;
};

extern int Run;
extern BYTE *Page[0x10];
extern BYTE IRAM[0x10000];
extern BYTE IO[0x100];
extern BYTE MemDummy[0x10000];
extern BYTE *ROMMap[0x100];     // C-ROMバンクマップ
extern int ROMBanks;            // C-ROMバンク数
extern BYTE *RAMMap[0x100];     // C-RAMバンクマップ
extern int RAMBanks;            // C-RAMバンク数
extern int RAMSize;             // C-RAM総容量
extern WORD IEep[64];
extern struct EEPROM sIEep;
extern struct EEPROM sCEep;

#define CK_EEP 1
extern int CartKind;

void WriteIO(DWORD A, BYTE V);
void WsReset (void);
void WsRomPatch(BYTE *buf);
void WsSetDir(void);
int WsRun(void);
void WsSplash(void);

#endif
