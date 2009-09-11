/*
$Date: 2009-09-10 11:00:27 +0900 (譛ｨ, 10 9 2009) $
$Rev: 5 $
*/

#include <time.h>
#include "WSRender.h"
#include "WS.h"
#include "WSDraw.h"
#include "WSInput.h"
#include "WSApu.h"
#include "WSFileio.h"
#include "WSPdata.h"
#include "WSBandai.h"
#include "WSError.h"
#include "nec/necintrf.h"

#define IPeriod 32          // HBlank/8 (256/8)

int Run;
BYTE *Page[16];             // バンク割り当て
BYTE IRAM[0x10000];         // 内部RAM 64kB = Page[0]
BYTE IO[0x100];             // IO
BYTE MemDummy[0x10000];     // ダミーバンク 64kB
BYTE *ROMMap[0x100];        // C-ROMバンクマップ
int ROMBanks;               // C-ROMバンク数
BYTE *RAMMap[0x100];        // C-RAMバンクマップ
int RAMBanks;               // C-RAMバンク数
int RAMSize;                // C-RAM総容量
WORD IEep[64];              // 内蔵EEPROM
struct EEPROM sIEep;        // EEPROM読み書き用構造体（内蔵）
struct EEPROM sCEep;        // EEPROM読み書き用構造体（カートリッジ）
int CartKind;               // セーブメモリの種類（CK_EEP = EEPROM）

static int ButtonState = 0x0000;    // Button state: B.A.START.OPTION.X4.X3.X2.X1.Y4.Y3.Y2.Y1
static WORD HTimer;
static WORD VTimer;
static int RtcCount;
static int RAMEnable;
static int FrameSkip = 0;
static int SkipCnt = 0;
static int TblSkip[5][5] = {
    {1,1,1,1,1},
    {0,1,1,1,1},
    {0,1,0,1,1},
    {0,0,1,0,1},
    {0,0,0,0,1},
};
#define MONO(C) 0xF000 | (C)<<8 | (C)<<4 | (C)
static WORD DefColor[] = {
    MONO(0xF), MONO(0xE), MONO(0xD), MONO(0xC), MONO(0xB), MONO(0xA), MONO(0x9), MONO(0x8),
    MONO(0x7), MONO(0x6), MONO(0x5), MONO(0x4), MONO(0x3), MONO(0x2), MONO(0x1), MONO(0x0)
};

void  ComEEP(struct EEPROM *eeprom, WORD *cmd, WORD *data)
{
    int i, j, op, addr;
    const int tblmask[16][5]=
    {
        {0x0000, 0, 0x0000, 0, 0x0000}, // dummy
        {0x0000, 0, 0x0000, 0, 0x0000},
        {0x0000, 0, 0x0000, 0, 0x0000},
        {0x0000, 0, 0x0000, 0, 0x0000},
        {0x000C, 2, 0x0003, 0, 0x0003},
        {0x0018, 3, 0x0006, 1, 0x0007},
        {0x0030, 4, 0x000C, 2, 0x000F},
        {0x0060, 5, 0x0018, 3, 0x001F},
        {0x00C0, 6, 0x0030, 4, 0x003F}, // 1Kbits IEEPROM
        {0x0180, 7, 0x0060, 5, 0x007F},
        {0x0300, 8, 0x00C0, 6, 0x00FF},
        {0x0600, 9, 0x0180, 7, 0x01FF},
        {0x0C00, 10, 0x0300, 8, 0x03FF}, // 16Kbits
        {0x1800, 11, 0x0600, 9, 0x07FF},
        {0x3000, 12, 0x0C00, 10, 0x0FFF},
        {0x6000, 13, 0x1800, 11, 0x1FFF},
    };
    if(eeprom->data == NULL)
    {
        return;
    }
    for(i = 15, j = 0x8000; i >= 0; i--, j >>= 1)
    {
        if(*cmd & j)
        {
            break;
        }
    }
    op = (*cmd & tblmask[i][0]) >> tblmask[i][1];
    switch(op)
    {
    case 0:
        addr = (*cmd & tblmask[i][2]) >> tblmask[i][3];
        switch(addr)
        {
        case 0:
            eeprom->we = 0;
            break;
        case 1:
            for(j = tblmask[i][4]; j >= 0; j--)
            {
                eeprom->data[j] = *data;
            }
            break;
        case 2:
            if(eeprom->we)
            {
                memset(eeprom->data, 0xFF, sizeof(eeprom->data)*2);
            }
            break;
        case 3:
            eeprom->we = 1;
            break;
        }
        *data = 0;
        break;
    case 1:
        if(eeprom->we)
        {
            addr = *cmd & tblmask[i][4];
            eeprom->data[addr] = *data;
            if (ROMBanks == 1 && addr == 0x3A) // アナザヘブンも書き込んでた
            {
                WsSplash();
                Run = 0; // パーソナルデータ最後の書き込みなので終了
            }
        }
        *data = 0;
        break;
    case 2:
        addr = *cmd & tblmask[i][4];
        *data = eeprom->data[addr];
        break;
    case 3:
        if(eeprom->we)
        {
            addr = *cmd & tblmask[i][4];
            eeprom->data[addr] = 0xFFFF;
        }
        *data = 0;
        break;
    default: break;
    }
}

BYTE  ReadMem(DWORD A)
{
    return Page[(A >> 16) & 0xF][A & 0xFFFF];
}

void  WriteMem(DWORD A, BYTE V)
{
    (*WriteMemFnTable[(A >> 16) & 0x0F])(A, V);
}

typedef void (*WriteMemFn)(DWORD A, BYTE V);

static void  WriteRom(DWORD A, BYTE V)
{
	ErrorMsg(ERR_WRITE_ROM);
}

static void  WriteIRam(DWORD A, BYTE V)
{
    IRAM[A & 0xFFFF] = V;
    if((A & 0xFE00) == 0xFE00)
    {
        SetPalette(A, V);
    }
    if(!((A - WaveMap) & 0xFFC0))
    {
        apuSetPData(A & 0x003F, V);
    }
}

static void  WriteCRam(DWORD A, BYTE V)
{
    if ((A & 0xFFFF) >= (unsigned)RAMSize)
    {
        ErrorMsg(ERR_OVER_RAMSIZE);
    }
    Page[1][A & 0xFFFF] = V;
}

void  WriteIO(DWORD A, BYTE V)
{
    int i, j, k;

    if(A >= 0x100)
    {
        return;
    }
    switch(A)
    {
    case 0x07:
        Scr1TMap = IRAM + ((V & 0x0F) << 11);
        Scr2TMap = IRAM + ((V & 0xF0) << 7);
        break;
    case 0x15:
        if (V & 0x01)
        {
            Segment[8] = 1;
			RenderSleep();
        }
		else
		{
            Segment[8] = 0;
		}
        if (V & 0x02)
        {
            SetDrawMode(1);
            Segment[4] = 1;
        }
        else
        {
            Segment[4] = 0;
        }
        if (V & 0x04)
        {
            SetDrawMode(0);
            Segment[3] = 1;
        }
        else
        {
            Segment[3] = 0;
        }
        if (V & 0x08)
        {
            Segment[2] = 1;
        }
		else
		{
            Segment[2] = 0;
		}
        if (V & 0x10)
        {
            Segment[1] = 1;
        }
		else
		{
            Segment[1] = 0;
		}
        if (V & 0x20)
        {
            Segment[0] = 1;
        }
		else
		{
            Segment[0] = 0;
		}
        break;
    case 0x1C:
    case 0x1D:
    case 0x1E:
    case 0x1F:
        if(COLCTL & 0x80) break;
        i = (A - 0x1C) << 1;
        MonoColor[i] = DefColor[V & 0x0F];
        MonoColor[i + 1] = DefColor[(V & 0xF0) >> 4];
        for(k = 0x20; k < 0x40; k++)
        {
            i = (k & 0x1E) >> 1;
            j = 0;
            if(k & 0x01) j = 2;
            Palette[i][j] = MonoColor[IO[k] & 0x07];
            Palette[i][j + 1] = MonoColor[(IO[k] >> 4) & 0x07];
        }
        break;
    case 0x20:
    case 0x21:
    case 0x22:
    case 0x23:
    case 0x24:
    case 0x25:
    case 0x26:
    case 0x27:
    case 0x28:
    case 0x29:
    case 0x2A:
    case 0x2B:
    case 0x2C:
    case 0x2D:
    case 0x2E:
    case 0x2F:
    case 0x30:
    case 0x31:
    case 0x32:
    case 0x33:
    case 0x34:
    case 0x35:
    case 0x36:
    case 0x37:
    case 0x38:
    case 0x39:
    case 0x3A:
    case 0x3B:
    case 0x3C:
    case 0x3D:
    case 0x3E:
    case 0x3F:
        if (COLCTL & 0x80) break;
        i = (A & 0x1E) >> 1;
        j = 0;
        if (A & 0x01) j = 2;
        Palette[i][j] = MonoColor[V & 0x07];
        Palette[i][j + 1] = MonoColor[(V >> 4) & 0x07];
        break;
    case 0x48:
        if(V & 0x80)
        {
            i = DMASRC;
            j = DMADST;
            k = DMACNT;
            while(k--)
            {
                WriteMem(j++, ReadMem(i++));
            }
            DMACNT = 0;
            DMASRC = i;
            DMADST = j;
            V &= 0x7F;
        }
        break;
    case 0x80:
    case 0x81:
        IO[A] = V;
        Ch[0].freq = *(unsigned short*)(IO + 0x80);
        return;
    case 0x82:
    case 0x83:
        IO[A] = V;
        Ch[1].freq = *(unsigned short*)(IO + 0x82);
        return;
    case 0x84:
    case 0x85:
        IO[A] = V;
        Ch[2].freq = *(unsigned short*)(IO + 0x84);
        return;
    case 0x86:
    case 0x87:
        IO[A] = V;
        Ch[3].freq = *(unsigned short*)(IO + 0x86);
        return;
    case 0x88:
        Ch[0].volL = (V >> 4) & 0x0F;
        Ch[0].volR = V & 0x0F;
        break;
    case 0x89:
        Ch[1].volL = (V >> 4) & 0x0F;
        Ch[1].volR = V & 0x0F;
        break;
    case 0x8A:
        Ch[2].volL = (V >> 4) & 0x0F;
        Ch[2].volR = V & 0x0F;
        break;
    case 0x8B:
        Ch[3].volL = (V >> 4) & 0x0F;
        Ch[3].volR = V & 0x0F;
        break;
    case 0x8C:
        Swp.step = (signed char)V;
        break;
    case 0x8D:
        Swp.time = (V + 1) << 5;
        break;
    case 0x8E:
        Noise.pattern = V & 0x07;
        break;
    case 0x8F:
        WaveMap = V << 6;
        for (i = 0; i < 64; i++) {
            apuSetPData(WaveMap + i, IRAM[WaveMap + i]);
        }
        break;
    case 0x90:
        Ch[0].on = V & 0x01;
        Ch[1].on = V & 0x02;
        Ch[2].on = V & 0x04;
        Ch[3].on = V & 0x08;
        VoiceOn   = V & 0x20;
        Swp.on    = V & 0x40;
        Noise.on  = V & 0x80;
        break;
    case 0x91:
        V |= 0x80; // ヘッドホンは常にオン
        break;
    case 0xA0:
        V=0x02;
        break;
    case 0xA2:
        if(V & 0x01)
        {
            HTimer = *(WORD*)(IO + 0xA4);
        }
        else
        {
            HTimer = 0;
        }
        if(V & 0x04)
        {
            VTimer = *(WORD*)(IO + 0xA6);
        }
        else
        {
            VTimer = 0;
        }
        break;
    case 0xA4:
    case 0xA5:
        break;
    case 0xA6:
    case 0xA7:
        IO[A + 4] = V;
        if(TIMCTL & 0x04)
        {
            VTimer = *(WORD*)(IO + 0xA6);
        }
        break;
    case 0xB3:
        if(V & 0x20)
        {
            V &= 0xDF;
        }
        V |= 0x04;
        break;
    case 0xB5:
        KEYCTL = (BYTE)(V & 0xF0);
        if(KEYCTL & 0x40) KEYCTL |= (BYTE)((ButtonState >> 8) & 0x0F);
        if(KEYCTL & 0x20) KEYCTL |= (BYTE)((ButtonState >> 4) & 0x0F);
        if(KEYCTL & 0x10) KEYCTL |= (BYTE)(ButtonState & 0x0F);
        return;
    case 0xB6:
        IRQACK &= (BYTE)~V;
        return;
    case 0xBE:
        ComEEP(&sIEep, (WORD*)(IO + 0xBC), (WORD*)(IO + 0xBA));
        V >>= 4;
        break;
    case 0xC0:
        if(nec_get_reg(NEC_CS) >= 0x4000)
        {
            nec_execute(1);
        }
        j = (V << 4) & 0xF0;
        Page[0x4] = ROMMap[0x4 | j];
        Page[0x5] = ROMMap[0x5 | j];
        Page[0x6] = ROMMap[0x6 | j];
        Page[0x7] = ROMMap[0x7 | j];
        Page[0x8] = ROMMap[0x8 | j];
        Page[0x9] = ROMMap[0x9 | j];
        Page[0xA] = ROMMap[0xA | j];
        Page[0xB] = ROMMap[0xB | j];
        Page[0xC] = ROMMap[0xC | j];
        Page[0xD] = ROMMap[0xD | j];
        Page[0xE] = ROMMap[0xE | j];
        Page[0xF] = ROMMap[0xF | j];
        break;
    case 0xC1:
        if(V >= RAMBanks) RAMEnable = 0;
        else Page[1] = RAMMap[V];
        break;
    case 0xC2:
        Page[2] = ROMMap[V];
        break;
    case 0xC3:
        Page[3] = ROMMap[V];
        break;
    case 0xC8:
        ComEEP(&sCEep, (WORD*)(IO + 0xC6), (WORD*)(IO + 0xC4));
        if(V & 0x10)
        {
            V >>= 4;
        }
        if(V & 0x20)
        {
            V >>= 4;
        }
        if(V & 0x40)
        {
            V >>= 5;
        }
        break;
    case 0xCA: // RTC Command
        if (V == 0x15)
        {
            RtcCount = 0;
        }
        break;
    case 0xCB: //RTC DATA
        break;
    default:
        break;
    }
    IO[A] = V;
}

#define  BCD(value) ((value / 10) << 4) | (value % 10)
BYTE ReadIO(DWORD A)
{
    switch(A)
    {
    case 0xCA:
        return IO[0xCA] | 0x80;
    case 0xCB:
        if (IO[0xCA] == 0x15)  // get time command
        { 
            BYTE year, mon, mday, wday, hour, min, sec, j;
            struct tm *newtime;
            time_t long_time;

            time(&long_time);
            newtime = localtime(&long_time);
            switch(RtcCount)
            {
            case 0:
                RtcCount++;
                year = newtime->tm_year;
                year %= 100;
                return BCD(year);
            case 1:
                RtcCount++;
                mon = newtime->tm_mon;
                mon++;
                return BCD(mon);
            case 2:
                RtcCount++;
                mday = newtime->tm_mday;
                return BCD(mday);
            case 3:
                RtcCount++;
                wday = newtime->tm_wday;
                return BCD(wday);
            case 4:
                RtcCount++;
                hour = newtime->tm_hour;
                j = BCD(hour);
                if (hour > 11)
                    j |= 0x80;
                return j;
            case 5:
                RtcCount++;
                min = newtime->tm_min;
                return BCD(min);
            case 6:
                RtcCount = 0;
                sec = newtime->tm_sec;
                return BCD(sec);
            }
            return 0;
        }
        else {
            // set ack
            return (IO[0xCB] | 0x80);
        }
    }
    return IO[A];
}

WriteMemFn WriteMemFnTable[16]= {
    WriteIRam,
    WriteCRam,
    WriteRom,
    WriteRom,
    WriteRom,
    WriteRom,
    WriteRom,
    WriteRom,
    WriteRom,
    WriteRom,
    WriteRom,
    WriteRom,
    WriteRom,
    WriteRom,
    WriteRom,
    WriteRom,
};

void WsReset (void)
{
    int i, j;

    Page[0x0] = IRAM;
    sIEep.data = IEep;
    sIEep.we = 0;
    if(CartKind & CK_EEP)
    {
        Page[0x1] = MemDummy;
        sCEep.data = (WORD*)(RAMMap[0x00]);
        sCEep.we = 0;
    }
    else
    {
        Page[0x1] = RAMMap[0x00];
        sCEep.data = NULL;
        sCEep.we = 0;
    }
    Page[0xF] = ROMMap[0xFF];
    i = (SPRTAB & 0x1F) << 9;
    i += SPRBGN << 2;
    j = SPRCNT << 2;
    memcpy(SprTMap, IRAM + i, j);
    SprTTMap = SprTMap;
    SprETMap = SprTMap + j - 4;
    WriteIO(0x07, 0x00);
    WriteIO(0x14, 0x01);
    WriteIO(0x1C, 0x99);
    WriteIO(0x1D, 0xFD);
    WriteIO(0x1E, 0xB7);
    WriteIO(0x1F, 0xDF);
    WriteIO(0x20, 0x30);
    WriteIO(0x21, 0x57);
    WriteIO(0x22, 0x75);
    WriteIO(0x23, 0x76);
    WriteIO(0x24, 0x15);
    WriteIO(0x25, 0x73);
    WriteIO(0x26, 0x77);
    WriteIO(0x27, 0x77);
    WriteIO(0x28, 0x20);
    WriteIO(0x29, 0x75);
    WriteIO(0x2A, 0x50);
    WriteIO(0x2B, 0x36);
    WriteIO(0x2C, 0x70);
    WriteIO(0x2D, 0x67);
    WriteIO(0x2E, 0x50);
    WriteIO(0x2F, 0x77);
    WriteIO(0x30, 0x57);
    WriteIO(0x31, 0x54);
    WriteIO(0x32, 0x75);
    WriteIO(0x33, 0x77);
    WriteIO(0x34, 0x75);
    WriteIO(0x35, 0x17);
    WriteIO(0x36, 0x37);
    WriteIO(0x37, 0x73);
    WriteIO(0x38, 0x50);
    WriteIO(0x39, 0x57);
    WriteIO(0x3A, 0x60);
    WriteIO(0x3B, 0x77);
    WriteIO(0x3C, 0x70);
    WriteIO(0x3D, 0x77);
    WriteIO(0x3E, 0x10);
    WriteIO(0x3F, 0x73);
    WriteIO(0x01, 0x00);
    WriteIO(0x8F, 0x03);
    WriteIO(0x91, 0x80);
    WriteIO(0xA0, 0x02);
    WriteIO(0xB3, 0x04);
    WriteIO(0xBA, 0x01);
    WriteIO(0xBB, 0x00);
    WriteIO(0xBC, 0x42);
    WriteIO(0xBD, 0x00);
    WriteIO(0xBE, 0x83);
    IO[0xC0] = 0x0F;
    j = 0xF0;
    Page[0x4] = ROMMap[0x4 | j];
    Page[0x5] = ROMMap[0x5 | j];
    Page[0x6] = ROMMap[0x6 | j];
    Page[0x7] = ROMMap[0x7 | j];
    Page[0x8] = ROMMap[0x8 | j];
    Page[0x9] = ROMMap[0x9 | j];
    Page[0xA] = ROMMap[0xA | j];
    Page[0xB] = ROMMap[0xB | j];
    Page[0xC] = ROMMap[0xC | j];
    Page[0xD] = ROMMap[0xD | j];
    Page[0xE] = ROMMap[0xE | j];
    Page[0xF] = ROMMap[0xF | j];
    WriteIO(0xC2, 0xFF);
    WriteIO(0xC3, 0xFF);
    IRAM[0x75AC]=0x41;
    IRAM[0x75AD]=0x5F;
    IRAM[0x75AE]=0x43;
    IRAM[0x75AF]=0x31;
    IRAM[0x75B0]=0x6E;
    IRAM[0x75B1]=0x5F;
    IRAM[0x75B2]=0x63;
    IRAM[0x75B3]=0x31;
    apuWaveClear();
    ButtonState = 0x0000;
	for (i = 0; i < 11; i++)
	{
		Segment[i] = 0;
	}
    nec_reset(NULL);
    nec_set_reg(NEC_SP, 0x2000);
}

void WsRomPatch(BYTE *buf)
{
    if((buf[0] == 0x01) && (buf[1] == 0x01) && (buf[2] == 0x16)) // SWJ-BANC16 STAR HEARTS
    {
        RAMBanks = 1;
        RAMSize = 0x8000;
        CartKind = 0;
    }
    if((buf[0] == 0x01) && (buf[1] == 0x00) && (buf[2] == 0x2C || buf[2] == 0x2F)) // SWJ-BAN02C,02F デジタルパートナー
    {
        RAMBanks = 1;
        RAMSize = 0x8000;
        CartKind = 0;
    }
    if((buf[0] == 0x01) && (buf[1] == 0x01) && (buf[2] == 0x38)) // SWJ-BANC38 NARUTO 木ノ葉忍法帖
    {
        RAMBanks = 1;
        RAMSize = 0x10000;
        CartKind = 0;
    }
}

int Interrupt(void)
{
    static int LCount=0, Joyz=0x0000;
    int i, j;

    if(++LCount>=8) // 8回で1Hblank期間
    {
        LCount=0;
    }
    switch(LCount)
    {
        case 0:
            if(RSTRL == 144)
            {
                DWORD VCounter;

                ButtonState = (WORD)Joystick();
                if((ButtonState ^ Joyz) & Joyz)
                {
                    if(IRQENA & KEY_IFLAG)
                    {
                        IRQACK |= KEY_IFLAG;
                    }
                }
                Joyz = ButtonState;
                // Vblankカウントアップ
                VCounter = *PVCNTH << 16 || *PVCNTL;
                VCounter++;
                *PVCNTL = (WORD)VCounter;
                *PVCNTH = (WORD)(VCounter >> 16);
            }
            break;
        case 2:
            // Hblank毎に1サンプルセットすることで12KHzのwaveデータが出来る
            apuWaveSet();
            break;
        case 4:
            if(RSTRL == 140)
            {
                i = (SPRTAB & 0x1F) << 9;
                i += SPRBGN << 2;
                j = SPRCNT << 2;
                memcpy(SprTMap, IRAM + i, j);
                SprTTMap = SprTMap;
                SprETMap= SprTMap + j - 4;
            }

            if(LCDSLP & 0x01)
            {
                if(RSTRL == 0)
                {
                    SkipCnt--;
                    if(SkipCnt < 0)
                    {
                        SkipCnt = 4;
                    }
                }
                if(TblSkip[FrameSkip][SkipCnt])
                {
                    if(RSTRL < 144)
                    {
                        RefreshLine(RSTRL);
                    }
                    if(RSTRL == 144)
                    {
                        drawDraw();
                    }
                }
            }
            break;
        case 6:
            if((TIMCTL & 0x01) && HTimer)
            {
                HTimer--;
                if(!HTimer)
                {
                    if(TIMCTL & 0x02)
                    {
                        HTimer = *PHPRE;
                    }
                    if(IRQENA & HTM_IFLAG)
                    {
                        IRQACK |= HTM_IFLAG;
                    }
                }
            }
            else if(*PHPRE == 1)
            {
                if(IRQENA & HTM_IFLAG)
                {
                    IRQACK |= HTM_IFLAG;
                }
            }
            if((IRQENA & VBB_IFLAG) && (RSTRL == 144))
            {
                IRQACK |= VBB_IFLAG;
            }
            if((TIMCTL & 0x04) && (RSTRL == 144) && VTimer)
            {
                VTimer--;
                if(!VTimer)
                {
                    if(TIMCTL & 0x08)
                    {
                        VTimer = *PVPRE;
                    }
                    if(IRQENA & VTM_IFLAG)
                    {
                        IRQACK |= VTM_IFLAG;
                    }
                }
            }
            if((IRQENA & RST_IFLAG) && (RSTRL == RSTRLC))
            {
                IRQACK |= RST_IFLAG;
            }
            break;
        case 7:
            RSTRL++;
            if(RSTRL >= 159)
            {
                RSTRL = 0;
            }
            // Hblankカウントアップ
            (*PHCNT)++;
            break;
        default:
            break;
    }
    return IRQACK;
}

int WsRun(void)
{
    static int period = IPeriod;
    int i, cycle, iack, inum;
//  for(i = 0; i < 480; i++) //5ms
    for(i = 0; i < 159*8; i++) // 1/75s
    {
        cycle = nec_execute(period);
        period += IPeriod - cycle;
        if(Interrupt())
        {
            iack = IRQACK;
            for(inum = 7; inum >= 0; inum--)
            {
                if(iack & 0x80)
                {
                    break;
                }
                iack <<= 1;
            }
            nec_int((inum + IRQBSE) << 2);
        }
    }
    return 0;
}

#define POS_X (88)
#define POS_Y (32)
#define NAME_Y (96)
void WsSplash(void)
{
    int x, y, i, len, pos, n;
    WORD* p;
    char* name = (char*)(IEep + 0x30);

    // 背景を黒でクリア
    p = FrameBuffer[0] + 8;
    for (y = 0; y < 144; y++)
    {
        for (x = 0; x < 224; x++)
        {
            *p++ = 0xF000;
        }
        p += 32;
    }
    drawDraw();
    Sleep(300);
    apuStartupSound();
    Sleep(150);
	Segment[3] = 1;
	Segment[5] = 1;
	Segment[9] = 1;
	Segment[10] = 1;
    for (i = 0; i < 6; i++)
    {
        WORD color[6] = {0xFF00, 0xFF80, 0xFFF0, 0xF0F0, 0xF0FF, 0xF00F};
        // B,A,N,D,A,I,を1文字表示
        for (y = bandaiRect[i].top; y <= bandaiRect[i].bottom; y++)
        {
            p = FrameBuffer[POS_Y + y] + 8 + POS_X;
            for (x = bandaiRect[i].left; x <= bandaiRect[i].right; x++)
            {
                if (bandai[y][x] == 0xFFFF)
                {
                    p[x] = color[i];
                }
            }
        }
        drawDraw();
        Sleep(70);
        // 文字消去
        p = FrameBuffer[0] + 8;
        for (y = 0; y < 144; y++)
        {
            for (x = 0; x < 224; x++)
            {
                *p++ = 0xF000;
            }
            p += 32;
        }
    }
    drawDraw();
    Sleep(200);
    // 背景を白でクリア
    p = FrameBuffer[0] + 8;
    for (y = 0; y < 144; y++)
    {
        for (x = 0; x < 224; x++)
        {
            *p++ = 0xFFFF;
        }
        p += 32;
    }
    // BANDAIロゴ表示
    p = FrameBuffer[POS_Y] + 8 + POS_X;
    for (y = 0; y < BANDAI_Y; y++)
    {
        for (x = 0; x < BANDAI_X; x++)
        {
            *p++ = bandai[y][x];
        }
        p += (256-BANDAI_X);
    }
    drawDraw();
    // 所有者の名前表示
    for (len = 15; name[len] == 0 && len > 0; len--);
    pos = 108 - len * 4;
    for (n = 0; n <= len; n++)
    {
        for (i = 0; i <= len; i++)
        {
            BYTE ar = pdataFontArray[name[i]];
            BYTE* ch = pdataFont + ar * 8;
            for (y = 0; y < 8; y++)
            {
                BYTE font = *ch++;
                p = FrameBuffer[NAME_Y + y] + 8 + pos + (i * 8);
                for (x = 0; x < 8; x++)
                {
                    if (name[i] && (font & 0x80)) // スペースフォントは■になってるのでスキップ
                    {
                        if (n == i)
                        {
                            *p = 0xFFFF;
                        }
                        else
                        {
                            *p = 0xF000;
                        }
                    }
                    p++;
                    font <<= 1;
                }
            }
        }
        drawDraw();
        Sleep(30);
    }
    for (i = 0; i <= len; i++)
    {
        BYTE ar = pdataFontArray[name[i]];
        BYTE* ch = pdataFont + ar * 8;
        for (y = 0; y < 8; y++)
        {
            BYTE font = *ch++;
            p = FrameBuffer[NAME_Y + y] + 8 + pos + (i * 8);
            for (x = 0; x < 8; x++)
            {
                if (name[i] && (font & 0x80))
                {
                    if (n == i)
                    {
                        *p = 0xFFFF;
                    }
                    else
                    {
                        *p = 0xF000;
                    }
                }
                p++;
                font <<= 1;
            }
        }
    }
	Segment[3] = 0;
	Segment[5] = 0;
	Segment[9] = 0;
    drawDraw();
}


