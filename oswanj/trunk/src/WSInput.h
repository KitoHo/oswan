/*
$Date: 2009-09-10 11:00:27 +0900 (木, 10 9 2009) $
$Rev: 5 $
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
