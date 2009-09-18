/*
$Date$
$Rev$
*/

#include "WSInput.h"

int WsJoypadH[12];
int WsJoypadV[12];
int WsKeyboardH[12];
int WsKeyboardV[12];
LPDIRECTINPUTDEVICE8 lpKeyDevice;
LPDIRECTINPUTDEVICE8 lpJoyDevice;
static int* WsJoypad;
static int* WsKeyboard;
static LPDIRECTINPUT8 lpDInput;
static DIJOYSTATE2 js;

BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* lpddi, LPVOID lpContext)
{
	HRESULT hRet;

	hRet = lpDInput->CreateDevice(lpddi->guidInstance, &lpJoyDevice, NULL);
	if(FAILED(hRet))  return DIENUM_CONTINUE;

	return DIENUM_STOP;
}

int WsInputJoyInit(HWND hw)
{
	HRESULT hRet;
	if (lpJoyDevice != NULL)
		return TRUE;

	hRet = lpDInput->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, NULL, DIEDFL_ATTACHEDONLY);
	if (hRet != DI_OK){
		if (lpJoyDevice != NULL)
		{
			lpJoyDevice->Release();
			lpJoyDevice = NULL;
		}
		return FALSE;
	}
	if (lpJoyDevice == NULL)
	{
		return FALSE;
	}
	hRet = lpJoyDevice->SetDataFormat(&c_dfDIJoystick2);
	if (hRet != DI_OK){
		if (lpJoyDevice != NULL)
		{
			lpJoyDevice->Release();
			lpJoyDevice = NULL;
		}
		return FALSE;
	}
	hRet = lpJoyDevice->SetCooperativeLevel(hw, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
	if (hRet != DI_OK){
		if (lpJoyDevice != NULL)
		{
			lpJoyDevice->Release();
			lpJoyDevice = NULL;
		}
		return FALSE;
	}
	lpJoyDevice->Acquire();

	return TRUE;
}

int WsInputInit(HWND hw)
{
	DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&lpDInput, NULL);
	lpDInput->CreateDevice(GUID_SysKeyboard, &lpKeyDevice, NULL);
	lpKeyDevice->SetDataFormat(&c_dfDIKeyboard);
	lpKeyDevice->SetCooperativeLevel(hw, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	return WsInputJoyInit(hw);
}

void WsInputJoyRelease(void)
{
	if (lpJoyDevice != NULL)
	{
		lpJoyDevice->Unacquire();
		lpJoyDevice->Release();
		lpJoyDevice = NULL;
	}
}

void WsInputRelease(void)
{
	if (lpKeyDevice != NULL)
	{
		lpKeyDevice->Unacquire();
		lpKeyDevice->Release();
		lpKeyDevice = NULL;
	}
	if (lpDInput != NULL)
	{
		lpDInput->Release();
		lpDInput = NULL;
	}
	WsInputJoyRelease();
}

#define HALFRECT JOY_POVRIGHT / 2
int WsInputCheckJoy(int value)
{
	int i;

	if ((value >= 1) && (value <= 128))
	{
		return((js.rgbButtons[value - 1] & 0x80) ? 1 : 0);
	}
	if ((value & 0x1100) == 0x100)
	{
		i = (value & 0x30) >> 4;
		switch (value & 0x0F)
		{
		case 1:
			if (js.rgdwPOV[i] == JOY_POVLEFT + HALFRECT) return 1;
			if (js.rgdwPOV[i] == JOY_POVFORWARD) return 1;
			if (js.rgdwPOV[i] == JOY_POVFORWARD + HALFRECT) return 1;
			break;
		case 2:
			if (js.rgdwPOV[i] == JOY_POVFORWARD + HALFRECT) return 1;
			if (js.rgdwPOV[i] == JOY_POVRIGHT) return 1;
			if (js.rgdwPOV[i] == JOY_POVRIGHT + HALFRECT) return 1;
			break;
		case 4:
			if (js.rgdwPOV[i] == JOY_POVRIGHT + HALFRECT) return 1;
			if (js.rgdwPOV[i] == JOY_POVBACKWARD) return 1;
			if (js.rgdwPOV[i] == JOY_POVBACKWARD + HALFRECT) return 1;
			break;
		case 8:
			if (js.rgdwPOV[i] == JOY_POVBACKWARD + HALFRECT) return 1;
			if (js.rgdwPOV[i] == JOY_POVLEFT) return 1;
			if (js.rgdwPOV[i] == JOY_POVLEFT + HALFRECT) return 1;
			break;
		}
		return 0;
	}
	switch (value)
	{
	case WS_JOY_AXIS_X_P:
		if (js.lX > 0xC000) return 1;
		break;
	case WS_JOY_AXIS_X_M:
		if (js.lX < 0x4000) return 1;
		break;
	case WS_JOY_AXIS_Y_P:
		if (js.lY > 0xC000) return 1;
		break;
	case WS_JOY_AXIS_Y_M:
		if (js.lY < 0x4000) return 1;
		break;
	case WS_JOY_AXIS_Z_P:
		if (js.lZ > 0xC000) return 1;
		break;
	case WS_JOY_AXIS_Z_M:
		if (js.lZ < 0x4000) return 1;
		break;
	case WS_JOY_AXIS_RX_P:
		if (js.lRx > 0xC000) return 1;
		break;
	case WS_JOY_AXIS_RX_M:
		if (js.lRx < 0x4000) return 1;
		break;
	case WS_JOY_AXIS_RY_P:
		if (js.lRy > 0xC000) return 1;
		break;
	case WS_JOY_AXIS_RY_M:
		if (js.lRy < 0x4000) return 1;
		break;
	case WS_JOY_AXIS_RZ_P:
		if (js.lRz > 0xC000) return 1;
		break;
	case WS_JOY_AXIS_RZ_M:
		if (js.lRz < 0x4000) return 1;
		break;
	case WS_JOY_SLIDER1_P:
		if (js.rglSlider[0] > 0xC000) return 1;
		break;
	case WS_JOY_SLIDER1_M:
		if (js.rglSlider[0] < 0x4000) return 1;
		break;
	case WS_JOY_SLIDER2_P:
		if (js.rglSlider[1] > 0xC000) return 1;
		break;
	case WS_JOY_SLIDER2_M:
		if (js.rglSlider[1] < 0x4000) return 1;
		break;
	}
	return 0;
}

WORD WsInputGetState(void)
{
	int i;
	HRESULT hRet;
	BYTE diKeys[256];
	WORD JoyState = 0;
	WORD ButtonState = 0;

	ZeroMemory(&js, sizeof(DIJOYSTATE2));
	ZeroMemory(diKeys, 256);
	if (lpJoyDevice != NULL)
	{
		hRet = lpJoyDevice->Poll();
		if (FAILED(hRet))
		{
			hRet = lpJoyDevice->Acquire();
			while(hRet == DIERR_INPUTLOST)
				hRet = lpJoyDevice->Acquire();
			return 0;
		}

		hRet = lpJoyDevice->GetDeviceState(sizeof(DIJOYSTATE2), &js);
		if (hRet == DI_OK){
			for (i = 0; i < 12; i++)
			{
				JoyState <<= 1;
				JoyState |= WsInputCheckJoy(WsJoypad[i]);
			}
		}
	}

	hRet = lpKeyDevice->Acquire();
	if (hRet == DI_OK || hRet == S_FALSE)
	{
		hRet = lpKeyDevice->GetDeviceState(256, diKeys);
		if (hRet == DI_OK)
		{
			for (i = 0; i < 12; i++)
			{
				ButtonState <<= 1;
				if (diKeys[WsKeyboard[i]] & 0x80)
				{
					ButtonState |= 1;
				}
			}
		}
	}
	return JoyState | ButtonState;
}

void SetKeyMap(int mode)
{
	if (mode & 1)
	{
		WsJoypad = WsJoypadV;
		WsKeyboard = WsKeyboardV;
	}
	else
	{
		WsJoypad = WsJoypadH;
		WsKeyboard = WsKeyboardH;
	}
}
