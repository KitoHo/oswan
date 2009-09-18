/*
$Date$
$Rev$
*/

#include "../resource.h"
#include "WSDialog.h"
#include <commctrl.h>
#include <tchar.h>
#include "WSInput.h"
#include "keycode.h"

static HWND hDlg, hTab, hTabCtrl1, hTabCtrl2, hTabCtrl3, hTabCtrl4;
static LPTSTR JoyStr[] = {
	TEXT("POV1 UP"), TEXT("POV1 RIGHT"), TEXT("POV1 DOWN"), TEXT("POV1 LEFT"),
	TEXT("POV2 UP"), TEXT("POV2 RIGHT"), TEXT("POV2 DOWN"), TEXT("POV2 LEFT"),
	TEXT("POV3 UP"), TEXT("POV3 RIGHT"), TEXT("POV3 DOWN"), TEXT("POV3 LEFT"),
	TEXT("POV4 UP"), TEXT("POV4 RIGHT"), TEXT("POV4 DOWN"), TEXT("POV4 LEFT"),
	TEXT("Y -"), TEXT("X +"), TEXT("Y +"), TEXT("X -"), TEXT("Z +"), TEXT("Z -"),
	TEXT("RY -"), TEXT("RX +"), TEXT("RY +"), TEXT("RX -"), TEXT("RZ +"), TEXT("RZ -"),
	TEXT("Slider1 +"), TEXT("Slider1 -"), TEXT("Slider2 +"), TEXT("Slider2 -")
};

static int JoyInt[] = {
WS_JOY_POV1_UP, WS_JOY_POV1_RIGHT, WS_JOY_POV1_DOWN, WS_JOY_POV1_LEFT,
WS_JOY_POV2_UP, WS_JOY_POV2_RIGHT, WS_JOY_POV2_DOWN, WS_JOY_POV2_LEFT,
WS_JOY_POV3_UP, WS_JOY_POV3_RIGHT, WS_JOY_POV3_DOWN, WS_JOY_POV3_LEFT,
WS_JOY_POV4_UP, WS_JOY_POV4_RIGHT, WS_JOY_POV4_DOWN, WS_JOY_POV4_LEFT,
WS_JOY_AXIS_Y_M, WS_JOY_AXIS_X_P, WS_JOY_AXIS_Y_P, WS_JOY_AXIS_X_M, WS_JOY_AXIS_Z_P, WS_JOY_AXIS_Z_M,
WS_JOY_AXIS_RY_M, WS_JOY_AXIS_RX_P, WS_JOY_AXIS_RY_P, WS_JOY_AXIS_RX_M, WS_JOY_AXIS_RZ_P, WS_JOY_AXIS_RZ_M,
WS_JOY_SLIDER1_P, WS_JOY_SLIDER1_M, WS_JOY_SLIDER2_P, WS_JOY_SLIDER2_M
};

LPTSTR joyName(int joy)
{
	static TCHAR buf[8];
	int i;

	if (joy < 0x80)
	{
		_stprintf_s(buf, 8, TEXT("%d"), joy);
		return buf;
	}
	for (i = 0; i < 32; i++)
	{
		if (joy == JoyInt[i])
		{
			return JoyStr[i];
		}
	}
	return NULL;
}

LRESULT CALLBACK ConfProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	NMHDR *nm = (NMHDR *)lParam;

	switch (msg)
	{
	case WM_INITDIALOG:
		return TRUE;
	case WM_NOTIFY:
		// タブコントロールの選択されているタブが変更されたことを通知
		switch (nm->code)
		{
		case TCN_SELCHANGE:
			if (nm->hwndFrom == hTab)
			{
				switch (TabCtrl_GetCurSel(hTab))
				{
				case 0:
					ShowWindow(hTabCtrl1, SW_SHOW);
					ShowWindow(hTabCtrl2, SW_HIDE);
					ShowWindow(hTabCtrl3, SW_HIDE);
					ShowWindow(hTabCtrl4, SW_HIDE);
					break;
				case 1:
					ShowWindow(hTabCtrl1, SW_HIDE);
					ShowWindow(hTabCtrl2, SW_SHOW);
					ShowWindow(hTabCtrl3, SW_HIDE);
					ShowWindow(hTabCtrl4, SW_HIDE);
					break;
				case 2:
					ShowWindow(hTabCtrl1, SW_HIDE);
					ShowWindow(hTabCtrl2, SW_HIDE);
					ShowWindow(hTabCtrl3, SW_SHOW);
					ShowWindow(hTabCtrl4, SW_HIDE);
					break;
				case 3:
					ShowWindow(hTabCtrl1, SW_HIDE);
					ShowWindow(hTabCtrl2, SW_HIDE);
					ShowWindow(hTabCtrl3, SW_HIDE);
					ShowWindow(hTabCtrl4, SW_SHOW);
					break;
				}
				return TRUE;
			}
			break;
		}
		break;
	case WM_COMMAND:
		if(HIWORD(wParam) == BN_CLICKED)
		{
			switch (LOWORD(wParam))
			{
			case IDOK:
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			case IDCANCEL:
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
		}
		break;
	}
	return FALSE;
}

LRESULT CALLBACK TabCtrlProc1(HWND hCtrl, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_INITDIALOG:
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_Y1), keyName[WsKeyboardH[11]]);
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_Y2), keyName[WsKeyboardH[10]]);
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_Y3), keyName[WsKeyboardH[9]]);
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_Y4), keyName[WsKeyboardH[8]]);
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_X1), keyName[WsKeyboardH[7]]);
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_X2), keyName[WsKeyboardH[6]]);
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_X3), keyName[WsKeyboardH[5]]);
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_X4), keyName[WsKeyboardH[4]]);
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_START), keyName[WsKeyboardH[2]]);
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_A), keyName[WsKeyboardH[1]]);
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_B), keyName[WsKeyboardH[0]]);
		return TRUE;
	}
	return FALSE;
}

LRESULT CALLBACK TabCtrlProc2(HWND hCtrl, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_INITDIALOG:
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_Y1), keyName[WsKeyboardV[11]]);
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_Y2), keyName[WsKeyboardV[10]]);
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_Y3), keyName[WsKeyboardV[9]]);
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_Y4), keyName[WsKeyboardV[8]]);
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_X1), keyName[WsKeyboardV[7]]);
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_X2), keyName[WsKeyboardV[6]]);
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_X3), keyName[WsKeyboardV[5]]);
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_X4), keyName[WsKeyboardV[4]]);
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_START), keyName[WsKeyboardV[2]]);
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_A), keyName[WsKeyboardV[1]]);
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_B), keyName[WsKeyboardV[0]]);
		return TRUE;
	}
	return FALSE;
}

LRESULT CALLBACK TabCtrlProc3(HWND hCtrl, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_INITDIALOG:
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_Y1), joyName(WsJoypadH[11]));
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_Y2), joyName(WsJoypadH[10]));
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_Y3), joyName(WsJoypadH[9]));
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_Y4), joyName(WsJoypadH[8]));
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_X1), joyName(WsJoypadH[7]));
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_X2), joyName(WsJoypadH[6]));
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_X3), joyName(WsJoypadH[5]));
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_X4), joyName(WsJoypadH[4]));
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_START), joyName(WsJoypadH[2]));
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_A), joyName(WsJoypadH[1]));
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_B), joyName(WsJoypadH[0]));
		return TRUE;
	}
	return FALSE;
}

LRESULT CALLBACK TabCtrlProc4(HWND hCtrl, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_INITDIALOG:
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_Y1), joyName(WsJoypadV[11]));
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_Y2), joyName(WsJoypadV[10]));
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_Y3), joyName(WsJoypadV[9]));
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_Y4), joyName(WsJoypadV[8]));
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_X1), joyName(WsJoypadV[7]));
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_X2), joyName(WsJoypadV[6]));
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_X3), joyName(WsJoypadV[5]));
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_X4), joyName(WsJoypadV[4]));
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_START), joyName(WsJoypadV[2]));
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_A), joyName(WsJoypadV[1]));
		SetWindowText(GetDlgItem(hCtrl, IDC_EDIT_B), joyName(WsJoypadV[0]));
		return TRUE;
	}
	return FALSE;
}

void WsDlgConfInit(HWND hWnd)
{
	HINSTANCE hInst;
	TC_ITEM tc;
	RECT    rt;
	LPPOINT pt = (LPPOINT)&rt;

	InitCommonControls();
	hInst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
	hDlg = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG_CONFIG), hWnd, (DLGPROC)ConfProc);
	ShowWindow(hDlg, SW_SHOW);
	hTab = GetDlgItem(hDlg, IDC_TAB1);
	// タブコントロールにタブシートを挿入
	tc.mask = TCIF_TEXT;
	tc.pszText = TEXT("キー横");
	TabCtrl_InsertItem(hTab , 0, &tc);
	tc.mask = TCIF_TEXT;
	tc.pszText = TEXT("キー縦");
	TabCtrl_InsertItem(hTab , 1, &tc);
	tc.mask = TCIF_TEXT;
	tc.pszText = TEXT("コントローラー横");
	TabCtrl_InsertItem(hTab , 2, &tc);
	tc.mask = TCIF_TEXT;
	tc.pszText = TEXT("コントローラー縦");
	TabCtrl_InsertItem(hTab , 3, &tc);
	// タブに貼り付けるダイアログを生成
	hTabCtrl1 = CreateDialog(hInst, (LPCTSTR)IDD_CONFIG_TAB1, hTab, (DLGPROC)TabCtrlProc1);
	hTabCtrl2 = CreateDialog(hInst, (LPCTSTR)IDD_CONFIG_TAB2, hTab, (DLGPROC)TabCtrlProc2);
	hTabCtrl3 = CreateDialog(hInst, (LPCTSTR)IDD_CONFIG_TAB1, hTab, (DLGPROC)TabCtrlProc3);
	hTabCtrl4 = CreateDialog(hInst, (LPCTSTR)IDD_CONFIG_TAB2, hTab, (DLGPROC)TabCtrlProc4);
	// タブコントロールのクライアント領域の座標を取得
	GetClientRect(hTab, &rt);
	TabCtrl_AdjustRect(hTab, FALSE, &rt);
	// タブのウィンドウの位置とサイズを変更する
	MoveWindow(hTabCtrl1, rt.left, rt.top, rt.right - rt.left, rt.bottom - rt.top, FALSE);
	MoveWindow(hTabCtrl2, rt.left, rt.top, rt.right - rt.left, rt.bottom - rt.top, FALSE);
	MoveWindow(hTabCtrl3, rt.left, rt.top, rt.right - rt.left, rt.bottom - rt.top, FALSE);
	MoveWindow(hTabCtrl4, rt.left, rt.top, rt.right - rt.left, rt.bottom - rt.top, FALSE);
	// デフォルトでタブ1を表示
	ShowWindow(hTabCtrl1, SW_SHOW);
}
