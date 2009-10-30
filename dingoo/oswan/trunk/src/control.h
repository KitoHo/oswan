#ifndef __control_h__
#define __control_h__

#define CONTROL_POWER         0x00000080

#define CONTROL_BUTTON_A      0x80000000
#define CONTROL_BUTTON_B      0x00200000
#define CONTROL_BUTTON_X      0x00010000
#define CONTROL_BUTTON_Y      0x00000040
#define CONTROL_BUTTON_START  0x00000800
#define CONTROL_BUTTON_SELECT 0x00000400

#define CONTROL_TRIGGER_LEFT  0x00000100
#define CONTROL_TRIGGER_RIGHT 0x20000000

#define CONTROL_DPAD_UP       0x00100000
#define CONTROL_DPAD_DOWN     0x08000000
#define CONTROL_DPAD_LEFT     0x10000000
#define CONTROL_DPAD_RIGHT    0x00040000

typedef struct {
	int changed;
	int pressed;
} control_state;

extern void          control_init();
extern void          control_poll();
extern control_state control_check(unsigned long inControl);

#endif
