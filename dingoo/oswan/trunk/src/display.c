/*
$Date$
$Rev$
*/

#include "entry.h"
#include "display.h"

const int display_width  = 320;
const int display_height = 240;

void  display_init() {

}

void* display_addr() {
	return (void*)_lcd_get_frame();
}

void display_clear(int inRed, int inGreen, int inBlue) {
	unsigned short* tempPtr = display_addr();
	unsigned short  tempColor = (inBlue >> 3) | ((inGreen & 0xFC) << 3) | ((inRed & 0xF8) << 10);
	int i, j;
	for(i = 0; i < display_height; i++) {
		for(j = 0; j < display_width; j++) {
			*(tempPtr++) = tempColor;
		}
	}
}

void display_flip() {
	lcd_flip();
	__dcache_writeback_all();
	_lcd_set_frame();
}
