/*
$Date$
$Rev$
*/

#include "WSInput.h"
#include "control.h"

int WsInputGetState(int mode)
{
	int button = 0; // Button state: 0.0.0.0.B.A.START.OPTION  X4.X3.X2.X1.Y4.Y3.Y2.Y1

	control_poll();
	switch (mode)
	{
	case 0:
		button |= (control_check(CONTROL_TRIGGER_RIGHT).pressed)     ; // Y1
		button |= (control_check(CONTROL_BUTTON_X     ).pressed) << 1; // Y2
		button |= (control_check(CONTROL_BUTTON_Y     ).pressed) << 2; // Y3
		button |= (control_check(CONTROL_TRIGGER_LEFT ).pressed) << 3; // Y4
		button |= (control_check(CONTROL_DPAD_UP      ).pressed) << 4; // X1
		button |= (control_check(CONTROL_DPAD_RIGHT   ).pressed) << 5; // X2
		button |= (control_check(CONTROL_DPAD_DOWN    ).pressed) << 6; // X3
		button |= (control_check(CONTROL_DPAD_LEFT    ).pressed) << 7; // X4
		button |= (control_check(CONTROL_BUTTON_START ).pressed) << 9; // START
		button |= (control_check(CONTROL_BUTTON_A     ).pressed) << 10;// A
		button |= (control_check(CONTROL_BUTTON_B     ).pressed) << 11;// B
		break;
	case 1:
		button |= (control_check(CONTROL_BUTTON_X     ).pressed)     ; // Y1
		button |= (control_check(CONTROL_BUTTON_A     ).pressed) << 1; // Y2
		button |= (control_check(CONTROL_BUTTON_B     ).pressed) << 2; // Y3
		button |= (control_check(CONTROL_BUTTON_Y     ).pressed) << 3; // Y4
		button |= (control_check(CONTROL_DPAD_UP      ).pressed) << 4; // X1
		button |= (control_check(CONTROL_DPAD_RIGHT   ).pressed) << 5; // X2
		button |= (control_check(CONTROL_DPAD_DOWN    ).pressed) << 6; // X3
		button |= (control_check(CONTROL_DPAD_LEFT    ).pressed) << 7; // X4
		button |= (control_check(CONTROL_BUTTON_START ).pressed) << 9; // START
		button |= (control_check(CONTROL_TRIGGER_RIGHT).pressed) << 10;// A
		button |= (control_check(CONTROL_TRIGGER_LEFT ).pressed) << 11;// B
		break;
	}
	return button;
}
