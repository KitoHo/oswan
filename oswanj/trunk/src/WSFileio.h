/*
$Date: 2009-09-18 18:18:58 +0900 ( 18 9 2009) $
$Rev$
*/

#ifndef WSFILEIO_H_
#define WSFILEIO_H_

extern wchar_t IniPath[512];

int WsCreate(wchar_t *CartName);
void WsRelease(void);
void WsLoadIEep(void);
void WsSaveIEep(void);
void WsLoadState(int num);
void WsSaveState(int num);

#endif
