/*
$Date: 2009-09-10 12:20:04 +0900 (木, 10 9 2009) $
$Rev: 11 $
*/

#include <string.h>
#include "WSconfig.h"
#include "WSInput.h"

int Verbose;

void ConfigCreate(void)
{
	strcpy(KeyConfig[0],"W");
	strcpy(KeyConfig[1],"D");
	strcpy(KeyConfig[2],"S");
	strcpy(KeyConfig[3],"A");
	strcpy(KeyConfig[4],"UP");
	strcpy(KeyConfig[5],"RIGHT");
	strcpy(KeyConfig[6],"DOWN");
	strcpy(KeyConfig[7],"LEFT");
	strcpy(KeyConfig[8],"CONTROL");
	strcpy(KeyConfig[9],"RETURN");
	strcpy(KeyConfig[10],"X");
	strcpy(KeyConfig[11],"Z");

	strcpy(KeyConfig[12],"LEFT");
	strcpy(KeyConfig[13],"UP");
	strcpy(KeyConfig[14],"RIGHT");
	strcpy(KeyConfig[15],"DOWN");
	strcpy(KeyConfig[16],"A");
	strcpy(KeyConfig[17],"W");
	strcpy(KeyConfig[18],"D");
	strcpy(KeyConfig[19],"S");
	strcpy(KeyConfig[20],"CONTROL");
	strcpy(KeyConfig[21],"RETURN");
	strcpy(KeyConfig[22],"X");
	strcpy(KeyConfig[23],"Z");
}

void ConfigRelease(void)
{
}
