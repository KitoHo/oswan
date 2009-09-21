/*
$Date$
$Rev$
*/

#ifndef WSDIALOG_H_
#define WSDIALOG_H_

#include <windows.h>

void WsDlgConfInit(HWND hWnd);
LRESULT CALLBACK ConfProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK EditProc1(HWND hEditWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK EditProc2(HWND hEditWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK EditProc3(HWND hEditWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK TabCtrlProc1(HWND hCtrl, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK TabCtrlProc2(HWND hCtrl, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK TabCtrlProc3(HWND hCtrl, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK TabCtrlProc4(HWND hCtrl, UINT msg, WPARAM wParam, LPARAM lParam);
LPTSTR GetJoyName(int joy);
int GetJoyState(void);

#endif
