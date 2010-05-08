/*
$Date: 2009-09-17 16:24:11 +0900 (木, 17 9 2009) $
$Rev$
*/

#include <windows.h>
#include <stdio.h>
#include "WSconfig.h"
#include "WSInput.h"
#include "WSFileio.h"

static LPCWSTR KeyName[] = {
	L"B", L"A", L"START", L"OPTION", 
	L"X4", L"X3", L"X2", L"X1", 
	L"Y4", L"Y3", L"Y2", L"Y1"
};

void ConfigCreate(void)
{
	int i;

	WsKeyboardH[0] = DIK_Z; // B
	WsKeyboardH[1] = DIK_X; // A
	WsKeyboardH[2] = DIK_RETURN; // START
	WsKeyboardH[3] = DIK_LCONTROL; // OPTION
	WsKeyboardH[4] = DIK_LEFT; // X4
	WsKeyboardH[5] = DIK_DOWN; // X3
	WsKeyboardH[6] = DIK_RIGHT; // X2
	WsKeyboardH[7] = DIK_UP; // X1
	WsKeyboardH[8] = DIK_A; // Y4
	WsKeyboardH[9] = DIK_S; // Y3
	WsKeyboardH[10] = DIK_D; // Y2
	WsKeyboardH[11] = DIK_W; // Y1

	WsKeyboardV[0] = DIK_Z; // B
	WsKeyboardV[1] = DIK_X; // A
	WsKeyboardV[2] = DIK_RETURN; // START
	WsKeyboardV[3] = DIK_LCONTROL; // OPTION
	WsKeyboardV[4] = DIK_S; // X4
	WsKeyboardV[5] = DIK_D; // X3
	WsKeyboardV[6] = DIK_W; // X2
	WsKeyboardV[7] = DIK_A; // X1
	WsKeyboardV[8] = DIK_DOWN; // Y4
	WsKeyboardV[9] = DIK_RIGHT; // Y3
	WsKeyboardV[10] = DIK_UP; // Y2
	WsKeyboardV[11] = DIK_LEFT; // Y1
	
	WsJoypadH[0] = 1; // B
	WsJoypadH[1] = 2; // A
	WsJoypadH[2] = 8; // START
	WsJoypadH[3] = 7; // OPTION
	WsJoypadH[4] = WS_JOY_AXIS_X_M; // X4
	WsJoypadH[5] = WS_JOY_AXIS_Y_P; // X3
	WsJoypadH[6] = WS_JOY_AXIS_X_P; // X2
	WsJoypadH[7] = WS_JOY_AXIS_Y_M; // X1
	WsJoypadH[8] = WS_JOY_POV1_LEFT; // Y4
	WsJoypadH[9] = WS_JOY_POV1_DOWN; // Y3
	WsJoypadH[10] = WS_JOY_POV1_RIGHT; // Y2
	WsJoypadH[11] = WS_JOY_POV1_UP; // Y1
	
	WsJoypadV[0] = 5; // B
	WsJoypadV[1] = 6; // A
	WsJoypadV[2] = 8; // START
	WsJoypadV[3] = 7; // OPTION
	WsJoypadV[4] = 1; // X4
	WsJoypadV[5] = 2; // X3
	WsJoypadV[6] = 4; // X2
	WsJoypadV[7] = 3; // X1
	WsJoypadV[8] = WS_JOY_AXIS_Y_P; // Y4
	WsJoypadV[9] = WS_JOY_AXIS_X_P; // Y3
	WsJoypadV[10] = WS_JOY_AXIS_Y_M; // Y2
	WsJoypadV[11] = WS_JOY_AXIS_X_M; // Y1
	
	for (i = 11; i >= 0; i--)
	{
		 WsKeyboardH[i] = GetPrivateProfileIntW(L"KEY_H", KeyName[i] , WsKeyboardH[i] , IniPath);
	}
	for (i = 11; i >= 0; i--)
	{
		WsKeyboardV[i] = GetPrivateProfileIntW(L"KEY_V", KeyName[i] , WsKeyboardV[i] , IniPath);
	}
	for (i = 11; i >= 0; i--)
	{
		WsJoypadH[i] = GetPrivateProfileIntW(L"JOY_H", KeyName[i] , WsJoypadH[i] , IniPath);
	}
	for (i = 11; i >= 0; i--)
	{
		WsJoypadV[i] = GetPrivateProfileIntW(L"JOY_V", KeyName[i] , WsJoypadV[i] , IniPath);
	}
}

static void WritePrivateProfileIntW(LPCWSTR lpAppName, LPCWSTR lpKeyName, int nInt, LPCWSTR lpFileName)
{
	wchar_t s[32];

	swprintf_s(s, 32, L"%d", nInt);
	WritePrivateProfileStringW(lpAppName, lpKeyName, s, lpFileName);
}

void ConfigRelease(void)
{
	int i;

	for (i = 11; i >= 0; i--)
	{
		WritePrivateProfileIntW(L"KEY_H", KeyName[i] , WsKeyboardH[i] , IniPath);
	}
	for (i = 11; i >= 0; i--)
	{
		WritePrivateProfileIntW(L"KEY_V", KeyName[i] , WsKeyboardV[i] , IniPath);
	}
	for (i = 11; i >= 0; i--)
	{
		WritePrivateProfileIntW(L"JOY_H", KeyName[i] , WsJoypadH[i] , IniPath);
	}
	for (i = 11; i >= 0; i--)
	{
		WritePrivateProfileIntW(L"JOY_V", KeyName[i] , WsJoypadV[i] , IniPath);
	}
}
