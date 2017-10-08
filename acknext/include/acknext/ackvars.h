#ifndef _ACKNEXT_ACKVARS_H_
#define _ACKNEXT_ACKVARS_H_

#include "config.h"
#include "ackmath.h"

ACKVAR ACKCONST SIZE screen_size;
ACKVAR var screen_gamma;

ACKVAR EVENT * ACKCONST on_resize;

ACKVAR EVENT * ACKCONST on_early_update; // Called before any other update logic, even before input
ACKVAR EVENT * ACKCONST on_update; // Called before any other update logic, but after input
ACKVAR EVENT * ACKCONST on_late_update; // Called after all update logic, but before drawing

ACKVAR EVENT * ACKCONST on_shutdown; // Invoked when the engine is beeing closed

// Time vars:

ACKVAR ACKCONST int total_frames;
ACKVAR ACKCONST var total_time;
ACKVAR ACKCONST var time_step;

// Color vars:
ACKVAR ACKCONST COLOR COLOR_BLACK;
ACKVAR ACKCONST COLOR COLOR_RED;
ACKVAR ACKCONST COLOR COLOR_GREEN;
ACKVAR ACKCONST COLOR COLOR_BLUE;
ACKVAR ACKCONST COLOR COLOR_YELLOW;
ACKVAR ACKCONST COLOR COLOR_MAGENTA;
ACKVAR ACKCONST COLOR COLOR_CYAN;
ACKVAR ACKCONST COLOR COLOR_WHITE;
ACKVAR ACKCONST COLOR COLOR_TRANSPARENT;

#endif // _ACKNEXT_ACKVARS_H_
