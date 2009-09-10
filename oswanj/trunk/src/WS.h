/*
$Date: 2009-09-10 11:00:27 +0900 (木, 10 9 2009) $
$Rev: 5 $
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
extern BYTE *ROMMap[0x100];
extern BYTE *RAMMap[0x100];
extern BYTE IRAM[0x10000];
extern BYTE IO[0x100];
extern BYTE MemDummy[0x10000];
extern BYTE *ROMMap[0x100];     // C-ROM�o���N�}�b�v
extern int ROMBanks;            // C-ROM�o���N��
extern BYTE *RAMMap[0x100];     // C-RAM�o���N�}�b�v
extern int RAMBanks;            // C-RAM�o���N��
extern int RAMSize;             // C-RAM���e��
extern WORD IEep[64];
extern struct EEPROM sIEep;
extern struct EEPROM sCEep;

#define CK_EEP 1
extern int CartKind;

void WsReset (void);
void WsRomPatch(BYTE *buf);
void WsSetDir(void);
int WsRun(void);
void WsSplash(void);

#endif
