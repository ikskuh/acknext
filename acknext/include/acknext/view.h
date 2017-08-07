#ifndef _ACKNEXT_VIEW_H_
#define _ACKNEXT_VIEW_H_

#include "config.h"
#include "ackmath.h"

typedef void (*RENDERCALL)(void * context);

typedef struct
{
	RENDERCALL renderer;
	void * context;

	POINT position;
	SIZE size;
	var layer;

	BITFIELD flags;
} VIEW;

ACKFUN VIEW * view_create(RENDERCALL render, void * context);

ACKFUN void view_draw(VIEW * view);

ACKFUN void view_remove(VIEW * view);

ACKFUN void view_to_bounds(VIEW const * view, POINT * pt, SIZE * size); // get view size

#endif // _ACKNEXT_VIEW_H_
