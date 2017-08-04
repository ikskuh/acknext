#ifndef INPUT_H
#define INPUT_H

#include "config.h"
#include "math.h"
#include "event.h"

typedef int BUTTONSTATE;

#define PRESSED  1
#define RELEASED 0

// Keyboard input

#define _ACKNEXT_KEYDEF(_name,_sym,_scancode)
#include "keyboard-config.h"
#undef _ACKNEXT_KEYDEF

// Mouse input

ACKVAR ACKCONST POINT mouse_pos;
ACKVAR ACKCONST VECTOR4 mouse_delta;

ACKVAR ACKCONST BUTTONSTATE mouse_left;
ACKVAR ACKCONST BUTTONSTATE mouse_middle;
ACKVAR ACKCONST BUTTONSTATE mouse_right;
ACKVAR ACKCONST BUTTONSTATE mouse_x1;
ACKVAR ACKCONST BUTTONSTATE mouse_x2;

ACKVAR EVENT * ACKCONST on_mouse_left;
ACKVAR EVENT * ACKCONST on_mouse_middle;
ACKVAR EVENT * ACKCONST on_mouse_right;
ACKVAR EVENT * ACKCONST on_mouse_x1;
ACKVAR EVENT * ACKCONST on_mouse_x2;

#endif // INPUT_H
