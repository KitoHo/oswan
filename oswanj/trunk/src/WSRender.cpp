﻿/*
$Date$
$Rev$
*/

#include <string.h>
#include "WSRender.h"
#include "WS.h"
#include "WSDraw.h"
#include "WSSegment.h"

#define MAP_TILE 0x01FF
#define MAP_PAL  0x1E00
#define MAP_BANK 0x2000
#define MAP_HREV 0x4000
#define MAP_VREV 0x8000
BYTE *Scr1TMap;
BYTE *Scr2TMap;

#define SPR_TILE 0x01FF
#define SPR_PAL  0x0E00
#define SPR_CLIP 0x1000
#define SPR_LAYR 0x2000
#define SPR_HREV 0x4000
#define SPR_VREV 0x8000
BYTE *SprTTMap;
BYTE *SprETMap;
BYTE SprTMap[512];

WORD Palette[16][16];
WORD MonoColor[8];
WORD FrameBuffer[144][LINE_SIZE];
WORD SegmentBuffer[(144 * 4) * (8 * 4)]; // 8 * 144 の4倍のサイズで描画
int Layer[3] = {1, 1, 1};
int Segment[11];

void SetPalette(int addr)
{
    WORD color;

    // RGB444 format
    color = *(WORD*)(IRAM + (addr & 0xFFFE));
    Palette[(addr & 0x1E0) >> 5][(addr & 0x1E) >> 1] = color | 0xF000;
}

void RefreshLine(int Line)
{
    WORD *pSBuf;            // データ書き込みバッファ
    WORD *pSWrBuf;          // ↑の書き込み位置用ポインタ
    int *pZ;                // ↓のインクリメント用ポインタ
    int ZBuf[0x100];        // FGレイヤーの非透明部を保存
    int *pW;                // ↓のインクリメント用ポインタ
    int WBuf[0x100];        // FGレイヤーのウィンドーを保存
    int OffsetX;            // 
    int OffsetY;            // 
    BYTE *pbTMap;           // 
    int TMap;               // 
    int TMapX;              // 
    int TMapXEnd;           // 
    BYTE *pbTData;          // 
    int PalIndex;               // 
    int i, j, k, index[8];
    WORD BaseCol;           // 
    pSBuf = FrameBuffer[Line] + 8;
    pSWrBuf = pSBuf;

    if(LCDSLP & 0x01)
    {
        if(COLCTL & 0xE0)
        {
            BaseCol = Palette[(BORDER & 0xF0) >> 4][BORDER & 0x0F];
        }
        else
        {
            BaseCol = MonoColor[BORDER & 0x07];
        }
    }
    else
    {
        BaseCol = 0;
    }
    for(i = 0; i < 224; i++)
    {
        {
            *pSWrBuf++ = BaseCol;
        }
    }
    if(!(LCDSLP & 0x01)) return;
/*********************************************************************/
    if((DSPCTL & 0x01) && Layer[0])                                 //BG layer
    {
        OffsetX = SCR1X & 0x07;
        pSWrBuf = pSBuf - OffsetX;
        i = Line + SCR1Y;
        OffsetY = (i & 0x07);

        pbTMap = Scr1TMap + ((i & 0xF8) << 3);
        TMapX = (SCR1X & 0xF8) >> 2;
        TMapXEnd = ((SCR1X + 224 + 7) >> 2) & 0xFFE;

        for(; TMapX < TMapXEnd;)
        {
            TMap = *(pbTMap + (TMapX++ & 0x3F));
            TMap |= *(pbTMap + (TMapX++ & 0x3F)) << 8;

            if(COLCTL & 0x40) // 16 colors
            {
                if(TMap & MAP_BANK)
                {
                    pbTData = IRAM + 0x8000;
                }
                else
                {
                    pbTData = IRAM + 0x4000;
                }
                pbTData += (TMap & MAP_TILE) << 5;
                if(TMap & MAP_VREV)
                {
                    pbTData += (7 - OffsetY) << 2;
                }
                else
                {
                    pbTData += OffsetY << 2;
                }
            }
            else
            {
                if((COLCTL & 0x80) && (TMap & MAP_BANK))// 4 colors and bank 1
                {
                    pbTData = IRAM + 0x4000;
                }
                else
                {
                    pbTData = IRAM + 0x2000;
                }
                pbTData += (TMap & MAP_TILE) << 4;
                if(TMap & MAP_VREV)
                {
                    pbTData += (7 - OffsetY) << 1;
                }
                else
                {
                    pbTData += OffsetY << 1;
                }
            }

            if(COLCTL & 0x20)                       // Packed Mode
            {
                if(COLCTL & 0x40)                   // 16 Color
                {
                    index[0] = (pbTData[0] & 0xF0) >> 4;
                    index[1] = pbTData[0] & 0x0F;
                    index[2] = (pbTData[1] & 0xF0) >> 4;
                    index[3] = pbTData[1] & 0x0F;
                    index[4] = (pbTData[2] & 0xF0) >> 4;
                    index[5] = pbTData[2] & 0x0F;
                    index[6] = (pbTData[3] & 0xF0) >> 4;
                    index[7] = pbTData[3] & 0x0F;
                }
                else                            // 4 Color
                {
                    index[0] = (pbTData[0] & 0xC0) >> 6;
                    index[1] = (pbTData[0] & 0x30) >> 4;
                    index[2] = (pbTData[0] & 0x0C) >> 2;
                    index[3] = pbTData[0] & 0x03;
                    index[4] = (pbTData[1] & 0xC0) >> 6;
                    index[5] = (pbTData[1] & 0x30) >> 4;
                    index[6] = (pbTData[1] & 0x0C) >> 2;
                    index[7] = pbTData[1] & 0x03;
                }
            }
            else
            {
                if(COLCTL & 0x40)                   // 16 Color
                {
                    index[0]  = (pbTData[0] & 0x80) ? 0x1 : 0;
                    index[0] |= (pbTData[1] & 0x80) ? 0x2 : 0;
                    index[0] |= (pbTData[2] & 0x80) ? 0x4 : 0;
                    index[0] |= (pbTData[3] & 0x80) ? 0x8 : 0;
                    index[1]  = (pbTData[0] & 0x40) ? 0x1 : 0;
                    index[1] |= (pbTData[1] & 0x40) ? 0x2 : 0;
                    index[1] |= (pbTData[2] & 0x40) ? 0x4 : 0;
                    index[1] |= (pbTData[3] & 0x40) ? 0x8 : 0;
                    index[2]  = (pbTData[0] & 0x20) ? 0x1 : 0;
                    index[2] |= (pbTData[1] & 0x20) ? 0x2 : 0;
                    index[2] |= (pbTData[2] & 0x20) ? 0x4 : 0;
                    index[2] |= (pbTData[3] & 0x20) ? 0x8 : 0;
                    index[3]  = (pbTData[0] & 0x10) ? 0x1 : 0;
                    index[3] |= (pbTData[1] & 0x10) ? 0x2 : 0;
                    index[3] |= (pbTData[2] & 0x10) ? 0x4 : 0;
                    index[3] |= (pbTData[3] & 0x10) ? 0x8 : 0;
                    index[4]  = (pbTData[0] & 0x08) ? 0x1 : 0;
                    index[4] |= (pbTData[1] & 0x08) ? 0x2 : 0;
                    index[4] |= (pbTData[2] & 0x08) ? 0x4 : 0;
                    index[4] |= (pbTData[3] & 0x08) ? 0x8 : 0;
                    index[5]  = (pbTData[0] & 0x04) ? 0x1 : 0;
                    index[5] |= (pbTData[1] & 0x04) ? 0x2 : 0;
                    index[5] |= (pbTData[2] & 0x04) ? 0x4 : 0;
                    index[5] |= (pbTData[3] & 0x04) ? 0x8 : 0;
                    index[6]  = (pbTData[0] & 0x02) ? 0x1 : 0;
                    index[6] |= (pbTData[1] & 0x02) ? 0x2 : 0;
                    index[6] |= (pbTData[2] & 0x02) ? 0x4 : 0;
                    index[6] |= (pbTData[3] & 0x02) ? 0x8 : 0;
                    index[7]  = (pbTData[0] & 0x01) ? 0x1 : 0;
                    index[7] |= (pbTData[1] & 0x01) ? 0x2 : 0;
                    index[7] |= (pbTData[2] & 0x01) ? 0x4 : 0;
                    index[7] |= (pbTData[3] & 0x01) ? 0x8 : 0;
                }
                else                            // 4 Color
                {
                    index[0]  = (pbTData[0] & 0x80) ? 0x1 : 0;
                    index[0] |= (pbTData[1] & 0x80) ? 0x2 : 0;
                    index[1]  = (pbTData[0] & 0x40) ? 0x1 : 0;
                    index[1] |= (pbTData[1] & 0x40) ? 0x2 : 0;
                    index[2]  = (pbTData[0] & 0x20) ? 0x1 : 0;
                    index[2] |= (pbTData[1] & 0x20) ? 0x2 : 0;
                    index[3]  = (pbTData[0] & 0x10) ? 0x1 : 0;
                    index[3] |= (pbTData[1] & 0x10) ? 0x2 : 0;
                    index[4]  = (pbTData[0] & 0x08) ? 0x1 : 0;
                    index[4] |= (pbTData[1] & 0x08) ? 0x2 : 0;
                    index[5]  = (pbTData[0] & 0x04) ? 0x1 : 0;
                    index[5] |= (pbTData[1] & 0x04) ? 0x2 : 0;
                    index[6]  = (pbTData[0] & 0x02) ? 0x1 : 0;
                    index[6] |= (pbTData[1] & 0x02) ? 0x2 : 0;
                    index[7]  = (pbTData[0] & 0x01) ? 0x1 : 0;
                    index[7] |= (pbTData[1] & 0x01) ? 0x2 : 0;
                }
            }

            if(TMap & MAP_HREV)
            {
                j = index[0];
                index[0] = index[7];
                index[7] = j;
                j = index[1];
                index[1] = index[6];
                index[6] = j;
                j = index[2];
                index[2] = index[5];
                index[5] = j;
                j = index[3];
                index[3] = index[4];
                index[4] = j;
            }

            PalIndex = (TMap & MAP_PAL) >> 9;
            if((!index[0]) && (!(!(COLCTL & 0x40) && (!(TMap & 0x0800))))) pSWrBuf++;
            else
            {
                *pSWrBuf++ = Palette[PalIndex][index[0]];
            }
            if((!index[1]) && (!(!(COLCTL & 0x40) && (!(TMap & 0x0800))))) pSWrBuf++;
            else
            {
                *pSWrBuf++ = Palette[PalIndex][index[1]];
            }
            if((!index[2]) && (!(!(COLCTL & 0x40) && (!(TMap & 0x0800))))) pSWrBuf++;
            else
            {
                *pSWrBuf++ = Palette[PalIndex][index[2]];
            }
            if((!index[3]) && (!(!(COLCTL & 0x40) && (!(TMap & 0x0800))))) pSWrBuf++;
            else
            {
                *pSWrBuf++ = Palette[PalIndex][index[3]];
            }
            if((!index[4]) && (!(!(COLCTL & 0x40) && (!(TMap & 0x0800))))) pSWrBuf++;
            else
            {
                *pSWrBuf++ = Palette[PalIndex][index[4]];
            }
            if((!index[5]) && (!(!(COLCTL & 0x40) && (!(TMap & 0x0800))))) pSWrBuf++;
            else
            {
                *pSWrBuf++ = Palette[PalIndex][index[5]];
            }
            if((!index[6]) && (!(!(COLCTL & 0x40) && (!(TMap & 0x0800))))) pSWrBuf++;
            else
            {
                *pSWrBuf++ = Palette[PalIndex][index[6]];
            }
            if((!index[7]) && (!(!(COLCTL & 0x40) && (!(TMap & 0x0800))))) pSWrBuf++;
            else
            {
                *pSWrBuf++ = Palette[PalIndex][index[7]];
            }
        }
    }
/*********************************************************************/
    memset(ZBuf, 0, sizeof(ZBuf));
    if((DSPCTL & 0x02) && Layer[1])          //FG layer表示
    {
        if((DSPCTL & 0x30) == 0x20) // ウィンドウ内部のみに表示
        {
            for(i = 0, pW = WBuf + 8; i < 224; i++)
            {
                *pW++ = 1;
            }
            if((Line >= SCR2WT) && (Line <= SCR2WB))
            {
                for(i = SCR2WL, pW = WBuf + 8 + i; (i <= SCR2WR) && (i < 224); i++)
                {
                    *pW++ = 0;
                }
            }
        }
        else if((DSPCTL & 0x30) == 0x30) // ウィンドウ外部のみに表示
        {
            for(i = 0, pW = WBuf + 8; i < 224; i++)
            {
                *pW++ = 0;
            }
            if((Line >= SCR2WT) && (Line <= SCR2WB))
            {
                for(i = SCR2WL, pW = WBuf + 8 + i; (i <= SCR2WR) && (i < 224); i++)
                {
                    *pW++ = 1;
                }
            }
        }
        else
        {
            for(i = 0, pW = WBuf + 8; i < 224; i++)
            {
                *pW++ = 0;
            }
        }

        OffsetX = SCR2X & 0x07;
        pSWrBuf = pSBuf - OffsetX;
        i = Line + SCR2Y;
        OffsetY = (i & 0x07);

        pbTMap = Scr2TMap + ((i & 0xF8) << 3);
        TMapX = (SCR2X & 0xF8) >> 2;
        TMapXEnd = ((SCR2X + 224 + 7) >> 2) & 0xFFE;

        pW = WBuf + 8 - OffsetX;
        pZ = ZBuf + 8 - OffsetX;
        
        for(; TMapX < TMapXEnd;)
        {
            TMap = *(pbTMap + (TMapX++ & 0x3F));
            TMap |= *(pbTMap + (TMapX++ & 0x3F)) << 8;

            if(COLCTL & 0x40)
            {
                if(TMap & MAP_BANK)
                {
                    pbTData = IRAM + 0x8000;
                }
                else
                {
                    pbTData = IRAM + 0x4000;
                }
                pbTData += (TMap & MAP_TILE) << 5;
                if(TMap & MAP_VREV)
                {
                    pbTData += (7 - OffsetY) << 2;
                }
                else
                {
                    pbTData += OffsetY << 2;
                }
            }
            else
            {
                if((COLCTL & 0x80) && (TMap & MAP_BANK))// 4 colors and bank 1
                {
                    pbTData = IRAM + 0x4000;
                }
                else
                {
                    pbTData = IRAM + 0x2000;
                }
                pbTData += (TMap & MAP_TILE) << 4;
                if(TMap & MAP_VREV)
                {
                    pbTData += (7 - OffsetY) << 1;
                }
                else
                {
                    pbTData += OffsetY << 1;
                }
            }

            if(COLCTL & 0x20)                       // Packed Mode
            {
                if(COLCTL & 0x40)                   // 16 Color
                {
                    index[0] = (pbTData[0] & 0xF0) >> 4;
                    index[1] = pbTData[0] & 0x0F;
                    index[2] = (pbTData[1] & 0xF0) >> 4;
                    index[3] = pbTData[1] & 0x0F;
                    index[4] = (pbTData[2] & 0xF0) >> 4;
                    index[5] = pbTData[2] & 0x0F;
                    index[6] = (pbTData[3] & 0xF0) >> 4;
                    index[7] = pbTData[3] & 0x0F;
                }
                else                            // 4 Color
                {
                    index[0] = (pbTData[0] & 0xC0) >> 6;
                    index[1] = (pbTData[0] & 0x30) >> 4;
                    index[2] = (pbTData[0] & 0x0C) >> 2;
                    index[3] = pbTData[0] & 0x03;
                    index[4] = (pbTData[1] & 0xC0) >> 6;
                    index[5] = (pbTData[1] & 0x30) >> 4;
                    index[6] = (pbTData[1] & 0x0C) >> 2;
                    index[7] = pbTData[1] & 0x03;
                }
            }
            else
            {
                if(COLCTL & 0x40)                   // 16 Color
                {
                    index[0]  = (pbTData[0] & 0x80) ? 0x1 : 0;
                    index[0] |= (pbTData[1] & 0x80) ? 0x2 : 0;
                    index[0] |= (pbTData[2] & 0x80) ? 0x4 : 0;
                    index[0] |= (pbTData[3] & 0x80) ? 0x8 : 0;
                    index[1]  = (pbTData[0] & 0x40) ? 0x1 : 0;
                    index[1] |= (pbTData[1] & 0x40) ? 0x2 : 0;
                    index[1] |= (pbTData[2] & 0x40) ? 0x4 : 0;
                    index[1] |= (pbTData[3] & 0x40) ? 0x8 : 0;
                    index[2]  = (pbTData[0] & 0x20) ? 0x1 : 0;
                    index[2] |= (pbTData[1] & 0x20) ? 0x2 : 0;
                    index[2] |= (pbTData[2] & 0x20) ? 0x4 : 0;
                    index[2] |= (pbTData[3] & 0x20) ? 0x8 : 0;
                    index[3]  = (pbTData[0] & 0x10) ? 0x1 : 0;
                    index[3] |= (pbTData[1] & 0x10) ? 0x2 : 0;
                    index[3] |= (pbTData[2] & 0x10) ? 0x4 : 0;
                    index[3] |= (pbTData[3] & 0x10) ? 0x8 : 0;
                    index[4]  = (pbTData[0] & 0x08) ? 0x1 : 0;
                    index[4] |= (pbTData[1] & 0x08) ? 0x2 : 0;
                    index[4] |= (pbTData[2] & 0x08) ? 0x4 : 0;
                    index[4] |= (pbTData[3] & 0x08) ? 0x8 : 0;
                    index[5]  = (pbTData[0] & 0x04) ? 0x1 : 0;
                    index[5] |= (pbTData[1] & 0x04) ? 0x2 : 0;
                    index[5] |= (pbTData[2] & 0x04) ? 0x4 : 0;
                    index[5] |= (pbTData[3] & 0x04) ? 0x8 : 0;
                    index[6]  = (pbTData[0] & 0x02) ? 0x1 : 0;
                    index[6] |= (pbTData[1] & 0x02) ? 0x2 : 0;
                    index[6] |= (pbTData[2] & 0x02) ? 0x4 : 0;
                    index[6] |= (pbTData[3] & 0x02) ? 0x8 : 0;
                    index[7]  = (pbTData[0] & 0x01) ? 0x1 : 0;
                    index[7] |= (pbTData[1] & 0x01) ? 0x2 : 0;
                    index[7] |= (pbTData[2] & 0x01) ? 0x4 : 0;
                    index[7] |= (pbTData[3] & 0x01) ? 0x8 : 0;
                }
                else                            // 4 Color
                {
                    index[0]  = (pbTData[0] & 0x80) ? 0x1 : 0;
                    index[0] |= (pbTData[1] & 0x80) ? 0x2 : 0;
                    index[1]  = (pbTData[0] & 0x40) ? 0x1 : 0;
                    index[1] |= (pbTData[1] & 0x40) ? 0x2 : 0;
                    index[2]  = (pbTData[0] & 0x20) ? 0x1 : 0;
                    index[2] |= (pbTData[1] & 0x20) ? 0x2 : 0;
                    index[3]  = (pbTData[0] & 0x10) ? 0x1 : 0;
                    index[3] |= (pbTData[1] & 0x10) ? 0x2 : 0;
                    index[4]  = (pbTData[0] & 0x08) ? 0x1 : 0;
                    index[4] |= (pbTData[1] & 0x08) ? 0x2 : 0;
                    index[5]  = (pbTData[0] & 0x04) ? 0x1 : 0;
                    index[5] |= (pbTData[1] & 0x04) ? 0x2 : 0;
                    index[6]  = (pbTData[0] & 0x02) ? 0x1 : 0;
                    index[6] |= (pbTData[1] & 0x02) ? 0x2 : 0;
                    index[7]  = (pbTData[0] & 0x01) ? 0x1 : 0;
                    index[7] |= (pbTData[1] & 0x01) ? 0x2 : 0;
                }
            }

            if(TMap & MAP_HREV)
            {
                j = index[0];
                index[0] = index[7];
                index[7] = j;
                j = index[1];
                index[1] = index[6];
                index[6] = j;
                j = index[2];
                index[2] = index[5];
                index[5] = j;
                j = index[3];
                index[3] = index[4];
                index[4] = j;
            }

            PalIndex = (TMap & MAP_PAL) >> 9;
            if(((!index[0]) && (!(!(COLCTL & 0x40) && (!(TMap & 0x0800))))) || (*pW)) pSWrBuf++;
            else
            {
                *pSWrBuf++ = Palette[PalIndex][index[0]];
                *pZ = 1;
            }
            pW++;pZ++;
            if(((!index[1]) && (!(!(COLCTL & 0x40) && (!(TMap & 0x0800))))) || (*pW)) pSWrBuf++;
            else
            {
                *pSWrBuf++ = Palette[PalIndex][index[1]];
                *pZ = 1;
            }
            pW++;pZ++;
            if(((!index[2]) && (!(!(COLCTL & 0x40) && (!(TMap & 0x0800))))) || (*pW)) pSWrBuf++;
            else
            {
                *pSWrBuf++ = Palette[PalIndex][index[2]];
                *pZ = 1;
            }
            pW++;pZ++;
            if(((!index[3]) && (!(!(COLCTL & 0x40) && (!(TMap & 0x0800))))) || (*pW)) pSWrBuf++;
            else
            {
                *pSWrBuf++ = Palette[PalIndex][index[3]];
                *pZ = 1;
            }
            pW++;pZ++;
            if(((!index[4]) && (!(!(COLCTL & 0x40) && (!(TMap & 0x0800))))) || (*pW)) pSWrBuf++;
            else
            {
                *pSWrBuf++ = Palette[PalIndex][index[4]];
                *pZ = 1;
            }
            pW++;pZ++;
            if(((!index[5]) && (!(!(COLCTL & 0x40) && (!(TMap & 0x0800))))) || (*pW)) pSWrBuf++;
            else
            {
                *pSWrBuf++ = Palette[PalIndex][index[5]];
                *pZ = 1;
            }
            pW++;pZ++;
            if(((!index[6]) && (!(!(COLCTL & 0x40) && (!(TMap & 0x0800))))) || (*pW)) pSWrBuf++;
            else
            {
                *pSWrBuf++ = Palette[PalIndex][index[6]];
                *pZ = 1;
            }
            pW++;pZ++;
            if(((!index[7]) && (!(!(COLCTL & 0x40) && (!(TMap & 0x0800))))) || (*pW)) pSWrBuf++;
            else
            {
                *pSWrBuf++ = Palette[PalIndex][index[7]];
                *pZ = 1;
            }
            pW++;pZ++;
        }
    }
/*********************************************************************/
    if((DSPCTL & 0x04) && Layer[2])          //sprite
    {
        if (DSPCTL & 0x08)      //sprite window
        {
            for (i = 0, pW = WBuf + 8; i < 224; i++)
            {
                *pW++ = 1;
            }
            if ((Line >= SPRWT) && (Line <= SPRWB))
            {
                for (i = SPRWL, pW = WBuf + 8 + i; (i <= SPRWR) && (i < 224); i++)
                {
                    *pW++ = 0;
                }
            }
        }

        for (pbTMap = SprETMap; pbTMap >= SprTTMap; pbTMap -= 4) // 
        {
            TMap = pbTMap[0];
            TMap |= pbTMap[1] << 8;

            if (pbTMap[2] > 0xF8)
            {
                j = pbTMap[2] - 0x100;
            }
            else
            {
                j = pbTMap[2];
            }
            if (pbTMap[3] > 0xF8)
            {
                k = pbTMap[3] - 0x100;
            }
            else
            {
                k = pbTMap[3];
            }

            if (Line < j)
                continue;
            if (Line >= j + 8)
                continue;
            if (224 <= k)
                continue;

            i = k;
            pSWrBuf = pSBuf + i;

            if (COLCTL & 0x40)
            {
                pbTData = IRAM + 0x4000;
                pbTData += (TMap & SPR_TILE) << 5;
                if (TMap & SPR_VREV)
                {
                    pbTData += (7 - Line + j) << 2;
                }
                else
                {
                    pbTData += (Line - j) << 2;
                }
            }
            else
            {
                pbTData = IRAM + 0x2000;
                pbTData += (TMap & SPR_TILE) << 4;
                if (TMap & SPR_VREV)
                {
                    pbTData += (7 - Line + j) << 1;
                }
                else
                {
                    pbTData += (Line - j) << 1;
                }
            }

            if(COLCTL & 0x20)                       // Packed Mode
            {
                if(COLCTL & 0x40)                   // 16 Color
                {
                    index[0] = (pbTData[0] & 0xF0) >> 4;
                    index[1] =  pbTData[0] & 0x0F;
                    index[2] = (pbTData[1] & 0xF0) >> 4;
                    index[3] =  pbTData[1] & 0x0F;
                    index[4] = (pbTData[2] & 0xF0) >> 4;
                    index[5] =  pbTData[2] & 0x0F;
                    index[6] = (pbTData[3] & 0xF0) >> 4;
                    index[7] =  pbTData[3] & 0x0F;
                }
                else                            // 4 Color
                {
                    index[0] = (pbTData[0] & 0xC0) >> 6;
                    index[1] = (pbTData[0] & 0x30) >> 4;
                    index[2] = (pbTData[0] & 0x0C) >> 2;
                    index[3] =  pbTData[0] & 0x03;
                    index[4] = (pbTData[1] & 0xC0) >> 6;
                    index[5] = (pbTData[1] & 0x30) >> 4;
                    index[6] = (pbTData[1] & 0x0C) >> 2;
                    index[7] =  pbTData[1] & 0x03;
                }
            }
            else
            {
                if(COLCTL & 0x40)                   // 16 Color
                {
                    index[0]  = (pbTData[0] & 0x80) ? 0x1 : 0;
                    index[0] |= (pbTData[1] & 0x80) ? 0x2 : 0;
                    index[0] |= (pbTData[2] & 0x80) ? 0x4 : 0;
                    index[0] |= (pbTData[3] & 0x80) ? 0x8 : 0;
                    index[1]  = (pbTData[0] & 0x40) ? 0x1 : 0;
                    index[1] |= (pbTData[1] & 0x40) ? 0x2 : 0;
                    index[1] |= (pbTData[2] & 0x40) ? 0x4 : 0;
                    index[1] |= (pbTData[3] & 0x40) ? 0x8 : 0;
                    index[2]  = (pbTData[0] & 0x20) ? 0x1 : 0;
                    index[2] |= (pbTData[1] & 0x20) ? 0x2 : 0;
                    index[2] |= (pbTData[2] & 0x20) ? 0x4 : 0;
                    index[2] |= (pbTData[3] & 0x20) ? 0x8 : 0;
                    index[3]  = (pbTData[0] & 0x10) ? 0x1 : 0;
                    index[3] |= (pbTData[1] & 0x10) ? 0x2 : 0;
                    index[3] |= (pbTData[2] & 0x10) ? 0x4 : 0;
                    index[3] |= (pbTData[3] & 0x10) ? 0x8 : 0;
                    index[4]  = (pbTData[0] & 0x08) ? 0x1 : 0;
                    index[4] |= (pbTData[1] & 0x08) ? 0x2 : 0;
                    index[4] |= (pbTData[2] & 0x08) ? 0x4 : 0;
                    index[4] |= (pbTData[3] & 0x08) ? 0x8 : 0;
                    index[5]  = (pbTData[0] & 0x04) ? 0x1 : 0;
                    index[5] |= (pbTData[1] & 0x04) ? 0x2 : 0;
                    index[5] |= (pbTData[2] & 0x04) ? 0x4 : 0;
                    index[5] |= (pbTData[3] & 0x04) ? 0x8 : 0;
                    index[6]  = (pbTData[0] & 0x02) ? 0x1 : 0;
                    index[6] |= (pbTData[1] & 0x02) ? 0x2 : 0;
                    index[6] |= (pbTData[2] & 0x02) ? 0x4 : 0;
                    index[6] |= (pbTData[3] & 0x02) ? 0x8 : 0;
                    index[7]  = (pbTData[0] & 0x01) ? 0x1 : 0;
                    index[7] |= (pbTData[1] & 0x01) ? 0x2 : 0;
                    index[7] |= (pbTData[2] & 0x01) ? 0x4 : 0;
                    index[7] |= (pbTData[3] & 0x01) ? 0x8 : 0;
                }
                else                            // 4 Color
                {
                    index[0]  = (pbTData[0] & 0x80) ? 0x1 : 0;
                    index[0] |= (pbTData[1] & 0x80) ? 0x2 : 0;
                    index[1]  = (pbTData[0] & 0x40) ? 0x1 : 0;
                    index[1] |= (pbTData[1] & 0x40) ? 0x2 : 0;
                    index[2]  = (pbTData[0] & 0x20) ? 0x1 : 0;
                    index[2] |= (pbTData[1] & 0x20) ? 0x2 : 0;
                    index[3]  = (pbTData[0] & 0x10) ? 0x1 : 0;
                    index[3] |= (pbTData[1] & 0x10) ? 0x2 : 0;
                    index[4]  = (pbTData[0] & 0x08) ? 0x1 : 0;
                    index[4] |= (pbTData[1] & 0x08) ? 0x2 : 0;
                    index[5]  = (pbTData[0] & 0x04) ? 0x1 : 0;
                    index[5] |= (pbTData[1] & 0x04) ? 0x2 : 0;
                    index[6]  = (pbTData[0] & 0x02) ? 0x1 : 0;
                    index[6] |= (pbTData[1] & 0x02) ? 0x2 : 0;
                    index[7]  = (pbTData[0] & 0x01) ? 0x1 : 0;
                    index[7] |= (pbTData[1] & 0x01) ? 0x2 : 0;
                }
            }

            if (TMap & SPR_HREV)
            {
                j = index[0];
                index[0] = index[7];
                index[7] = j;
                j = index[1];
                index[1] = index[6];
                index[6] = j;
                j = index[2];
                index[2] = index[5];
                index[5] = j;
                j = index[3];
                index[3] = index[4];
                index[4] = j;
            }

            pW = WBuf + 8 + k;
            pZ = ZBuf + k + 8;
            PalIndex = ((TMap & SPR_PAL) >> 9) + 8;
            for(i = 0; i < 8; i++, pZ++, pW++)
            {
                if(DSPCTL & 0x08)
                {
                    if(TMap & SPR_CLIP)
                    {
                        if(!*pW)
                        {
                            pSWrBuf++;
                            continue;
                        }
                    }
                    else
                    {
                        if(*pW)
                        {
                            pSWrBuf++;
                            continue;
                        }
                    }
                }
                if((!index[i]) && (!(!(COLCTL & 0x40) && (!(TMap & 0x0800)))))
                {
                    pSWrBuf++;
                    continue;
                }
                if((*pZ) && (!(TMap & SPR_LAYR)))
                {
                    pSWrBuf++;
                    continue;
                }
                *pSWrBuf++ = Palette[PalIndex][index[i]];
            }
        }
    }
}

/*
 8 * 144 のサイズを 32 * 576 で描画
*/
void RenderSegment(void)
{
    int bit, x, y, i;
    WORD* p = SegmentBuffer;
    BYTE lcd[11] = {0x20, 0x10, 0x08, 0x04, 0x02, 0x40, 0, 0, 0x01, 0x80, 0}; // 大、中、小、横、縦、ヘッドホン、音量、電池、スリープ

    for (i = 0; i < 11; i++)
    {
        for (y = 0; y < segLine[i]; y++)
        {
            for (x = 0; x < 4; x++)
            {
                BYTE ch = seg[i][y * 4 + x];
                for (bit = 0; bit < 8; bit++)
                {
                    if (ch & 0x80)
                    {
                        if (Segment[i])
                        {
                            *p++ = 0xFCCC;
                        }
                        else
                        {
                            *p++ = 0xF222;
                        }
                    }
                    else
                    {
                        *p++ = 0xF000;
                    }
                    ch <<= 1;
                }
            }
        }
    }
}

void RenderSleep(void)
{
    int x, y;
    WORD* p;

    // 背景をグレイでクリア
    p = FrameBuffer[0] + 8;
    for (y = 0; y < 144; y++)
    {
        for (x = 0; x < 224; x++)
        {
            *p++ = 0xF444;
        }
        p += 32;
    }
    drawDraw();
}
