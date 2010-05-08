/*
$Date: 2009-09-16 21:35:44 +0900 (水, 16 9 2009) $
$Rev$
*/

#ifndef WSDRAW_H_
#define WSDRAW_H_

#define   DIRECTDRAW_VERSION 0x0900
#include <d3d9.h>
#include <d3dx9.h>

extern int Kerorikan;

enum DRAWSIZE {
    DS_1 = 1,
    DS_2 = 2,
    DS_3 = 3,
    DS_4 = 4,
    DS_FULL = 255
};

HRESULT drawInitialize(BOOL isFullScreen);
void    drawFinalize(void);
HRESULT drawCreate(void);
void    drawDestroy(void);
void    drawDraw(void);
void    WsResize(void);
int     SetDrawMode(int Mode);
int     SetDrawSize(enum DRAWSIZE Size);

#endif
