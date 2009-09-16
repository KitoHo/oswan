/*
$Date: 2009-09-10 12:20:04 +0900 (木, 10 9 2009) $
$Rev: 11 $
*/

#include <string.h>
#include "WSconfig.h"
#include "WSInput.h"

void ConfigCreate(void)
{
	// Button state: B.A.START.OPTION.X4.X3.X2.X1.Y4.Y3.Y2.Y1
	WsKeyboardH[0] = DIK_Z;
	WsKeyboardH[1] = DIK_X;
	WsKeyboardH[2] = DIK_RETURN;
	WsKeyboardH[3] = DIK_LCONTROL;
	WsKeyboardH[4] = DIK_LEFT;
	WsKeyboardH[5] = DIK_DOWN;
	WsKeyboardH[6] = DIK_RIGHT;
	WsKeyboardH[7] = DIK_UP;
	WsKeyboardH[8] = DIK_A;
	WsKeyboardH[9] = DIK_S;
	WsKeyboardH[10] = DIK_D;
	WsKeyboardH[11] = DIK_W;
	WsKeyboardV[0] = DIK_Z;
	WsKeyboardV[1] = DIK_X;
	WsKeyboardV[2] = DIK_RETURN;
	WsKeyboardV[3] = DIK_LCONTROL;
	WsKeyboardV[4] = DIK_S;
	WsKeyboardV[5] = DIK_D;
	WsKeyboardV[6] = DIK_W;
	WsKeyboardV[7] = DIK_A;
	WsKeyboardV[8] = DIK_DOWN;
	WsKeyboardV[9] = DIK_RIGHT;
	WsKeyboardV[10] = DIK_UP;
	WsKeyboardV[11] = DIK_LEFT;
}

void ConfigRelease(void)
{
}
