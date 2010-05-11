/*
$Date$
$Rev$
*/

#include <windows.h>
#include <tchar.h>
#include "WSError.h"

void  ErrorMsg(long err)
{
    TCHAR dst[512];

    _stprintf_s(dst, 512, TEXT("%lX"), err);
    MessageBox(NULL, dst, TEXT("WS エラー"), MB_OK);
}

