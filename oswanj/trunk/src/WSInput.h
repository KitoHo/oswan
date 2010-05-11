/*
$Date$
$Rev$
*/

#ifndef WSINPUT_H_
#define WSINPUT_H_

#define DIRECTINPUT_VERSION	0x0800

#include <windows.h>
#include <dinput.h>

#define WS_JOY_POV1_UP    0x0101
#define WS_JOY_POV1_RIGHT 0x0102
#define WS_JOY_POV1_DOWN  0x0104
#define WS_JOY_POV1_LEFT  0x0108
#define WS_JOY_POV2_UP    0x0111
#define WS_JOY_POV2_RIGHT 0x0112
#define WS_JOY_POV2_DOWN  0x0114
#define WS_JOY_POV2_LEFT  0x0118
#define WS_JOY_POV3_UP    0x0121
#define WS_JOY_POV3_RIGHT 0x0122
#define WS_JOY_POV3_DOWN  0x0124
#define WS_JOY_POV3_LEFT  0x0128
#define WS_JOY_POV4_UP    0x0131
#define WS_JOY_POV4_RIGHT 0x0132
#define WS_JOY_POV4_DOWN  0x0134
#define WS_JOY_POV4_LEFT  0x0138
#define WS_JOY_AXIS_X_P   0x1000
#define WS_JOY_AXIS_X_M   0x1001
#define WS_JOY_AXIS_Y_P   0x1002
#define WS_JOY_AXIS_Y_M   0x1003
#define WS_JOY_AXIS_Z_P   0x1004
#define WS_JOY_AXIS_Z_M   0x1005
#define WS_JOY_AXIS_RX_P  0x1006
#define WS_JOY_AXIS_RX_M  0x1007
#define WS_JOY_AXIS_RY_P  0x1008
#define WS_JOY_AXIS_RY_M  0x1009
#define WS_JOY_AXIS_RZ_P  0x100A
#define WS_JOY_AXIS_RZ_M  0x100B
#define WS_JOY_SLIDER1_P  0x100C
#define WS_JOY_SLIDER1_M  0x100D
#define WS_JOY_SLIDER2_P  0x100E
#define WS_JOY_SLIDER2_M  0x100F

extern int WsKeyboardH[];
extern int WsKeyboardV[];
extern int WsJoypadH[];
extern int WsJoypadV[];
extern LPDIRECTINPUTDEVICE8 lpKeyDevice;
extern LPDIRECTINPUTDEVICE8 lpJoyDevice;

int WsInputInit(HWND hw);
int WsInputJoyInit(HWND hw);
void WsInputJoyRelease(void);
void WsInputRelease(void);
int WsInputCheckJoy(int value);
WORD WsInputGetState(void);
void WsInputSetKeyMap(int mode);
int WsInputGetNowait(void);

#endif
