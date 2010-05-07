/*
$Date$
$Rev$
*/

#ifndef WSRENDER_H_
#define WSRENDER_H_

#include "WSHard.h"

#define LINE_SIZE (256)

extern BYTE *Scr1TMap;
extern BYTE *Scr2TMap;
extern BYTE *SprTTMap;
extern BYTE *SprETMap;
extern BYTE SprTMap[512];
extern WORD Palette[16][16];
extern WORD MonoColor[8];
extern WORD FrameBuffer[144][LINE_SIZE];
extern WORD SegmentBuffer[(144 * 4) * (8 * 4)];
extern int Layer[3];
extern int Segment[11];

void SetPalette(int addr);
void RefreshLine(int Line);
void RenderSegment(void);
void RenderSleep(void);

#endif
