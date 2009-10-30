/*
$Date$
$Rev$
*/

#ifndef WSFILEIO_H_
#define WSFILEIO_H_

void WsSetDir(char* path);
int WsCreate(char *CartName);
void WsRelease(void);
void WsLoadIEep(void);
void WsSaveIEep(void);
void WsLoadState(int num);
void WsSaveState(int num);

#endif
