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
	WsButtonsH[0] = DIK_Z;
	WsButtonsH[1] = DIK_X;
	WsButtonsH[2] = DIK_RETURN;
	WsButtonsH[3] = DIK_LCONTROL;
	WsButtonsH[4] = DIK_LEFT;
	WsButtonsH[5] = DIK_DOWN;
	WsButtonsH[6] = DIK_RIGHT;
	WsButtonsH[7] = DIK_UP;
	WsButtonsH[8] = DIK_A;
	WsButtonsH[9] = DIK_S;
	WsButtonsH[10] = DIK_D;
	WsButtonsH[11] = DIK_W;
	WsButtonsV[0] = DIK_Z;
	WsButtonsV[1] = DIK_X;
	WsButtonsV[2] = DIK_RETURN;
	WsButtonsV[3] = DIK_LCONTROL;
	WsButtonsV[4] = DIK_S;
	WsButtonsV[5] = DIK_D;
	WsButtonsV[6] = DIK_W;
	WsButtonsV[7] = DIK_A;
	WsButtonsV[8] = DIK_DOWN;
	WsButtonsV[9] = DIK_RIGHT;
	WsButtonsV[10] = DIK_UP;
	WsButtonsV[11] = DIK_LEFT;
}

void ConfigRelease(void)
{
}
