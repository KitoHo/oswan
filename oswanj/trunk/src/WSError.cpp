/*
$Date: 2009-09-10 11:00:27 +0900 (Êú®, 10 9 2009) $
$Rev: 5 $
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

