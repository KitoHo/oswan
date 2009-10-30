/*
$Date$
$Rev$
*/

#include "entry.h"
#include "control.h"

KEY_STATUS _control_status[2];

void control_init() {
	_kbd_get_status(_control_status);
	_control_status[1] = _control_status[0];
}

void control_poll() {
	_kbd_get_status(_control_status);
	_control_status[1] = _control_status[0];
}

control_state control_check(unsigned long inControl) {
	control_state tempOut;
	tempOut.changed = (((_control_status[0].status ^ _control_status[1].status) & inControl) != 0);
	tempOut.pressed = ((_control_status[0].status & inControl) != 0);
	return tempOut;
}
