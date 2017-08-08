#ifndef _ACKNEXT_ACKDEBUG_H_
#define ACKDEBUG_H

#include "config.h"
#include "ackmath.h"

ACKFUN void draw_line3d(
	VECTOR const * p1,
	VECTOR const * p2,
	COLOR const * color);

ACKFUN void draw_point3d(
	VECTOR const * pos,
	COLOR const * color);

#endif // ACKDEBUG_H
