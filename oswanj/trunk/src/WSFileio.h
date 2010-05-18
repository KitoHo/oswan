/*
$Date$
$Rev$
*/

#ifndef WSFILEIO_H_
#define WSFILEIO_H_

extern wchar_t IniPath[512];

int WsCreate(wchar_t *CartName);
void WsRelease(void);
void WsLoadEeprom(void);
void WsSaveEeprom(void);
void WsLoadState(int num);
void WsSaveState(int num);

#endif
