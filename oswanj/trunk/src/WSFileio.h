/*
$Date: 2009-09-10 11:00:27 +0900 (木, 10 9 2009) $
$Rev: 5 $
*/

#ifndef WSFILEIO_H_
#define WSFILEIO_H_

extern wchar_t IniPath[512];

int WsCreate(wchar_t *CartName);
void WsRelease(void);
void WsLoadIEep(void);
void WsSaveIEep(void);

#endif
