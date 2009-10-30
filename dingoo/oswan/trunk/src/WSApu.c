/*
$Date$
$Rev$
*/

#include "entry.h"
#include "WSHard.h"
#include "WS.h"
#include "WSApu.h"
#include "WSError.h"
#include "startup.h"

WAVEOUT_INFO mywaveinfo;

#define BUFSIZEN    0x10000
#define WAV_FREQ    12000
#define SND_BNKSIZE 256
#define SND_RNGSIZE (10 * SND_BNKSIZE)
#define WAV_VOLUME  20

unsigned long WaveMap;
SOUND Ch[4];
int VoiceOn;
SWEEP Swp;
NOISE Noise;
int Sound[7] = {1, 1, 1, 1, 1, 1, 1};

static unsigned char PData[4][32];
static unsigned char PDataN[8][BUFSIZEN];
static unsigned int  RandData[BUFSIZEN];

static short sndbuffer[SND_RNGSIZE][2]; // Sound Ring Buffer
static int   rBuf, wBuf;
static void* hWaveOut;
static int   StartupFlag;
static int   WaveTaskStk[256];
static BYTE  WsWaveVol = 10;

extern BYTE *Page[16];
extern BYTE IO[0x100];

int apuBufLen(void)
{
	if (wBuf >= rBuf) return wBuf - rBuf;
	return SND_RNGSIZE + wBuf - rBuf;
}

short* apuBufGetLock(int size)
{
	if (apuBufLen() >= size) {
		return &sndbuffer[rBuf][0];
	}
	return NULL;
}

void apuBufGetUnlock(void* ptr, int size)
{
	if (ptr == (void*)&sndbuffer[rBuf][0]) {
		rBuf = (rBuf + size) % SND_RNGSIZE;
	}
}

void apuWaveTask(void* pdata)
{
	short* src = NULL;

	for(;;)
	{
		if (StartupFlag)
		{
			int i;
			mywaveinfo.frequency = 11025;
			mywaveinfo.bits = 16;
			mywaveinfo.channels = 2;
			mywaveinfo.volume = 20;
			hWaveOut = waveout_open(&mywaveinfo);
			Sleep(10);
			for (i = 0; i < 36; i++)
			{
				waveout_write(hWaveOut, startup + i * (size_startup / 36), size_startup / 36);
			}
			Sleep(10);
			waveout_close(hWaveOut);
			StartupFlag = 0;
			mywaveinfo.frequency = WAV_FREQ;
			mywaveinfo.bits = 16;
			mywaveinfo.channels = 2;
			mywaveinfo.volume = WsWaveVol;
			hWaveOut = waveout_open(&mywaveinfo);
		}
		else
		{
			if (apuBufLen() < SND_BNKSIZE)
			{
				Sleep(1);
			}
			else
			{
				src = apuBufGetLock(SND_BNKSIZE);
				if (src)
				{
					waveout_write(hWaveOut, src, SND_BNKSIZE * 4);
					apuBufGetUnlock(src, SND_BNKSIZE);
					src = NULL;
				}
			}
		}
	}
}

void apuWaveVolume(int up)
{
	if (up)
	{
		WsWaveVol += 5;
		if (WsWaveVol > 50)
		{
			WsWaveVol = 50;
		}
	}
	else if (WsWaveVol >= 5)
	{
		WsWaveVol -= 5;
	}
	waveout_set_volume(WsWaveVol);
}

void apuWaveCreate(void)
{
	int ret;
	ret = OSTaskCreate(apuWaveTask, NULL, (void*)&WaveTaskStk[255], 16);
}

void apuWaveDel(void)
{
	waveout_close(hWaveOut);
	OSTaskDel(16);
}

void apuWaveClear(void)
{
}

int apuInit(void)
{
    int i, j;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 32; j++) {
            PData[i][j] = 8;
        }
    }
    for (i = 0; i < 8; i++) {
        for (j = 0; j < BUFSIZEN; j++) {
            PDataN[i][j] = ((apuMrand(15 - i) & 1) ? 15 : 0);
        }
    }
    for (i = 0; i < BUFSIZEN; i++) {
        RandData[i] = apuMrand(15);
    }
    rBuf = 0;
    wBuf = 0;
	apuWaveCreate();
    return 0;
}

void apuEnd(void)
{
	apuWaveDel();
}

unsigned int apuMrand(unsigned int Degree)
{
#define BIT(n) (1<<n)
    typedef struct {
        unsigned int N;
        int InputBit;
        int Mask;
    } POLYNOMIAL;

    static POLYNOMIAL TblMask[]=
    {
        { 2,BIT(2) ,BIT(0)|BIT(1)},
        { 3,BIT(3) ,BIT(0)|BIT(1)},
        { 4,BIT(4) ,BIT(0)|BIT(1)},
        { 5,BIT(5) ,BIT(0)|BIT(2)},
        { 6,BIT(6) ,BIT(0)|BIT(1)},
        { 7,BIT(7) ,BIT(0)|BIT(1)},
        { 8,BIT(8) ,BIT(0)|BIT(2)|BIT(3)|BIT(4)},
        { 9,BIT(9) ,BIT(0)|BIT(4)},
        {10,BIT(10),BIT(0)|BIT(3)},
        {11,BIT(11),BIT(0)|BIT(2)},
        {12,BIT(12),BIT(0)|BIT(1)|BIT(4)|BIT(6)},
        {13,BIT(13),BIT(0)|BIT(1)|BIT(3)|BIT(4)},
        {14,BIT(14),BIT(0)|BIT(1)|BIT(4)|BIT(5)},
        {15,BIT(15),BIT(0)|BIT(1)},
        { 0,      0,      0},
    };
    static POLYNOMIAL *pTbl = TblMask;
    static int ShiftReg = BIT(2)-1;
    int XorReg = 0;
    int Masked;

    if(pTbl->N != Degree) {
        pTbl = TblMask;
        while(pTbl->N) {
            if(pTbl->N == Degree) {
                break;
            }
            pTbl++;
        }
        if(!pTbl->N) {
            pTbl--;
        }
        ShiftReg &= pTbl->InputBit-1;
        if(!ShiftReg) {
            ShiftReg = pTbl->InputBit-1;
        }
    }
    Masked = ShiftReg & pTbl->Mask;
    while(Masked) {
        XorReg ^= Masked & 0x01;
        Masked >>= 1;
    }
    if(XorReg) {
        ShiftReg |= pTbl->InputBit;
    }
    else {
        ShiftReg &= ~pTbl->InputBit;
    }
    ShiftReg >>= 1;
    return ShiftReg;
}

void apuSetPData(int addr, unsigned char val)
{
    int i, j;

    i = (addr & 0x30) >> 4;
    j = (addr & 0x0F) << 1;
    PData[i][j]=(unsigned char)(val & 0x0F);
    PData[i][j + 1]=(unsigned char)((val & 0xF0)>>4);
}

unsigned char apuVoice(void)
{
    static int index = 0, b = 0;
    unsigned char v;

    if ((SDMACTL & 0x98) == 0x98) { // Hyper voice
        v = Page[SDMASB + b][SDMASA + index++];
        if ((SDMASA + index) == 0)
        {
            b++;
        }
        if (v < 0x80) {
            v += 0x80;
        }
        else {
            v -= 0x80;
        }
        if (SDMACNT <= index) {
            index = 0;
            b = 0;
        }
        return v;
    }
    else if ((SDMACTL & 0x88) == 0x80) { // DMA start
        IO[0x89] = Page[SDMASB + b][SDMASA + index++];
        if ((SDMASA + index) == 0)
        {
            b++;
        }
        if (SDMACNT <= index) {
            SDMACTL &= 0x7F; // DMA end
            SDMACNT = 0;
            index = 0;
            b = 0;
        }
    }
    return ((VoiceOn && Sound[4]) ? IO[0x89] : 0x80);
}

void apuSweep(void)
{
    if ((Swp.step) && Swp.on) { // sweep on
        if (Swp.cnt < 0) {
            Swp.cnt = Swp.time;
            Ch[2].freq += Swp.step;
            Ch[2].freq &= 0x7ff;
        }
        Swp.cnt--;
    }
}

WORD apuShiftReg(void)
{
    static int nPos = 0;
    // Noise counter
    if (++nPos >= BUFSIZEN) {
        nPos = 0;
    }
    return RandData[nPos];
}

void apuWaveSet(void)
{
    static  int point[] = {0, 0, 0, 0};
    static  int preindex[] = {0, 0, 0, 0};
    int     channel, index;
    short   value, lVol[4], rVol[4];
    short   LL, RR, vVol;

    apuSweep();
    for (channel = 0; channel < 4; channel++) {
        lVol[channel] = 0;
        rVol[channel] = 0;
        if (Ch[channel].on) {
            if (channel == 1 && VoiceOn && Sound[4]) {
                continue;
            }
            else if (channel == 2 && Swp.on && !Sound[5]) {
                continue;
            }
            else if (channel == 3 && Noise.on && Sound[6]){
                index = (3072000 / WAV_FREQ) * point[3] / (2048 - Ch[3].freq);
                if ((index %= BUFSIZEN) == 0 && preindex[3]) {
                    point[3] = 0;
                }
                value = (short)PDataN[Noise.pattern][index] - 8;
            }
            else if (Sound[channel] == 0) {
                continue;
            }
            else {
                index = (3072000 / WAV_FREQ) * point[channel] / (2048 - Ch[channel].freq);
                if ((index %= 32) == 0 && preindex[channel]) {
                    point[channel] = 0;
                }
                value = (short)PData[channel][index] - 8;
            }
            preindex[channel] = index;
            point[channel]++;
            lVol[channel] = value * Ch[channel].volL; // -8*15=-120, 7*15=105
            rVol[channel] = value * Ch[channel].volR;
        }
    }
    vVol = ((short)apuVoice() - 0x80);
    // mix 16bits wave -32768 ` +32767 32768/120 = 273
    LL = (lVol[0] + lVol[1] + lVol[2] + lVol[3] + vVol) * WAV_VOLUME;
    RR = (rVol[0] + rVol[1] + rVol[2] + rVol[3] + vVol) * WAV_VOLUME;
    sndbuffer[wBuf][0] = LL;
    sndbuffer[wBuf][1] = RR;
    if (++wBuf >= SND_RNGSIZE) {
        wBuf = 0;
    }
}

void apuStartupSound(void)
{
	StartupFlag = 1;
}
