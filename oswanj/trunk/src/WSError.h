/*
$Date$
$Rev$
*/

#ifndef WSERROR_H_
#define WSERROR_H_

#define ERR_MALLOC					0x80000001
#define ERR_OVER_RAMSIZE			0x80010001
#define ERR_WRITE_ROM				0x80010002
#define ERR_WAVEOUT_OPEN			0x80020001
#define ERR_WAVEOUT_PREPAREHEADER	0x80020002
#define ERR_FOPEN					0x80030001
#define ERR_FREAD_ROMINFO			0x80030002
#define ERR_ILLEGAL_ROMSIZE			0x80030003
#define ERR_FREAD_ROM				0x80030004
#define ERR_CHECKSUM				0x80030005
#define ERR_FREAD_SAVE				0x80030006
#define ERR_DRAW_HAL				0x80040001

void  ErrorMsg(long err);

#endif

