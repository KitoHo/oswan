/*
$Date: 2009-09-16 21:35:44 +0900 (Ê∞¥, 16 9 2009) $
$Rev$
*/

#include <windows.h>
#include <tchar.h>
#include "WSError.h"

void  ErrorMsg(long err)
{
    TCHAR dst[512];

    _stprintf_s(dst, 512, TEXT("%lX"), err);
    MessageBox(NULL, dst, TEXT("WS ÉGÉâÅ["), MB_OK);
}

