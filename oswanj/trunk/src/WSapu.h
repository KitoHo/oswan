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
extern unsigned char PData[4][32];
extern int Sound[7];
extern int WsWaveVol;

int apuWaveCreate(void);
void apuWaveRelease(void);
void apuWaveClear(void);
int apuInit(void);
void apuEnd(void);
unsigned int apuMrand(unsigned int Degree);
void apuSetPData(int addr, unsigned char val);
unsigned char apuVoice(void);
void apuSweep(void);
void apuWaveSet(void);
void apuStartupSound(void);

#endif
