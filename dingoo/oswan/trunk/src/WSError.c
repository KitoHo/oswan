/*
$Date$
$Rev$
*/

#include "entry.h"
#include "WSError.h"

void  ErrorMsg(long err)
{
    char dst[512];
	FILE* fp;

    sprintf(dst, "%08lX\n", err);
	fp = fsys_fopen("err.txt", "a");
    fsys_fwrite(dst, 1, 9, fp);
	fsys_fclose(fp);
}

