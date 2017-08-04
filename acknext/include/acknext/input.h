#ifndef INPUT_H
#define INPUT_H

#ifdef __gnu_linux__
// @0: include the definition of key_t here!
#include <sys/types.h>
#endif

#include "config.h"
#include "math.h"
#include "event.h"

typedef int BUTTONSTATE;

#define PRESSED  1
#define RELEASED 0

// @0: And now redefine key_t to our own symbol...
#define key_t ackkey_t

// Keyboard input
#define _ACKNEXT_KEYDEF(_name,_scancode) ACKVAR ACKCONST BUTTONSTATE key_##_name;
#include "keyboard-config.h"
#undef _ACKNEXT_KEYDEF

// Keyboard events
#define _ACKNEXT_KEYDEF(_name,_scancode) ACKVAR ACKCONST EVENT * on_##_name;
#include "keyboard-config.h"
#undef _ACKNEXT_KEYDEF

ACKVAR ACKCONST BUTTONSTATE key_any;
ACKVAR ACKCONST EVENT * on_anykey;

ACKVAR ACKCONST int key_lastpressed;

// Mouse input
ACKVAR ACKCONST POINT mouse_pos;
ACKVAR ACKCONST VECTOR4 mickey;

ACKVAR ACKCONST BUTTONSTATE mouse_left;
ACKVAR ACKCONST BUTTONSTATE mouse_middle;
ACKVAR ACKCONST BUTTONSTATE mouse_right;
ACKVAR ACKCONST BUTTONSTATE mouse_x1;
ACKVAR ACKCONST BUTTONSTATE mouse_x2;

// Mouse events
ACKVAR EVENT * ACKCONST on_mouse_left;
ACKVAR EVENT * ACKCONST on_mouse_middle;
ACKVAR EVENT * ACKCONST on_mouse_right;
ACKVAR EVENT * ACKCONST on_mouse_x1;
ACKVAR EVENT * ACKCONST on_mouse_x2;

// Functions
ACKFUN char const * str_for_key(int scancode);

ACKFUN int key_for_string(char const * keyname);

#endif // INPUT_H
