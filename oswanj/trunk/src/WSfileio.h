/*
$Date$
$Rev$
*/

#ifndef WSFILEIO_H_
#define WSFILEIO_H_

int WsCreate(wchar_t *CartName);
void WsRelease(void);
void WsLoadIEep(void);
void WsSaveIEep(void);

#endif
