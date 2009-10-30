/*
$Date$
$Rev$
*/

#include "entry.h"
#include "display.h"
#include "control.h"
#include "WSFileio.h"
#include "WS.h"
#include "WSApu.h"
#include "WSDraw.h"
#include "WSError.h"

extern int _sys_judge_event(void *);
extern char* __to_locale_ansi(wchar_t *);

unsigned short *g_pGameDecodeBuf = NULL;

int game_running = 1;

int GameMain(char* respath)
{
    int ref = 1;
    int sysref;
    char* fullpath;

    display_init();
    display_clear(0x00, 0x00, 0x00);
    display_flip();
    control_init();
    fullpath = __to_locale_ansi((wchar_t*)respath);
	control_poll();
	if(control_check(CONTROL_TRIGGER_LEFT).pressed)
	{
		fullpath = NULL;
	}
	WsSetDir(fullpath);
	if (WsCreate(fullpath) < 0)
	{
		return ref;
	}
	drawInit();
	apuInit();
	WsLoadIEep();
	WsSplash();
    while(game_running)
	{
        sysref = _sys_judge_event(NULL);
        if(sysref < 0)
		{
            ref = sysref;
            break;
        }
		control_poll();
		if(control_check(CONTROL_POWER).pressed)
		{
			break;
		}
		else if(control_check(CONTROL_BUTTON_SELECT).pressed)
		{
			static int up = 0;
			static int down = 0;
			if(control_check(CONTROL_TRIGGER_LEFT).pressed)
			{
				if (!down)
				{
					apuWaveVolume(0);
					down = 1;
				}
			}
			else
			{
				down = 0;
			}
			if(control_check(CONTROL_TRIGGER_RIGHT).pressed)
			{
				if (!up)
				{
					apuWaveVolume(10);
					up = 1;
				}
			}
			else
			{
				up = 0;
			}
		}
		else
		{
			WsRun();
		}
    }
	WsSaveIEep();
	WsRelease();
	apuEnd();
    return ref;
}
