/*
$Date$
$Rev$
*/

#ifndef WSINPUT_H_
#define WSINPUT_H_

#include "WSHard.h"

extern char KeyConfig[24][64];
extern char JoyConfig[24][64];

int WsInputInit(void);
int Joystick(void);
void WsKeyDown(WORD Key);
void WsKeyUp(WORD Key);
void SetKeyMap(int Mode);

#endif
