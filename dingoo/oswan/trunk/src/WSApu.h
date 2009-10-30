/*
$Date$
$Rev$
*/

#ifndef WSAPU_H_
#define WSWAPU_H_

typedef struct sound {
    int on;
    int freq;
    int volL;
    int volR;
} SOUND;
typedef struct sweep {
    int on;
    int time;
    int step;
    int cnt;
} SWEEP;
typedef struct noise {
    int on;
    int pattern;
} NOISE;

extern unsigned long WaveMap;
extern SOUND Ch[4];
extern int VoiceOn;
extern SWEEP Swp;
extern NOISE Noise;
extern int Sound[7];

int apuBufLen(void);
void apuWaveVolume(int);
void apuWaveCreate(void);
void apuWaveDel(void);
void apuWaveClear(void);
int apuInit(void);
void apuEnd(void);
unsigned int apuMrand(unsigned int);
void apuSetPData(int, unsigned char);
unsigned char apuVoice(void);
void apuSweep(void);
WORD apuShiftReg(void);
void apuWaveSet(void);
void apuStartupSound(void);

#endif
