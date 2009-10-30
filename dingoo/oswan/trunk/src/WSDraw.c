/*
$Date$
$Rev$
*/

#include "WSHard.h"
#include "WSDraw.h"
#include "WSRender.h"

#define SCREN_OFFSET (SCREEN_HEIGHT - LCD_MAIN_H) / 2 * SCREEN_WIDTH + (SCREEN_WIDTH - LCD_MAIN_W) / 2

extern void _lcd_set_frame();
extern unsigned short* _lcd_get_frame();
extern void lcd_flip();
extern void __dcache_writeback_all();

void drawInit(void)
{
	FrameBuffer = _lcd_get_frame() + SCREN_OFFSET;
}

void drawDraw()
{
	int i, j;
	WORD* p = FrameBuffer - 8;
	for (i = 0; i < LCD_MAIN_H; i++)
	{
		for (j = 0; j < 8; j++)
		{
			*p++ = 0;
		}
		p += LCD_MAIN_W;
		for (j = 0; j < 8; j++)
		{
			*p++ = 0;
		}
		p += SCREEN_WIDTH - LCD_MAIN_W - 16;
	}
	__dcache_writeback_all();
	_lcd_set_frame();
	lcd_flip();
	FrameBuffer = _lcd_get_frame() + SCREN_OFFSET;
}

