#ifndef _ACKNEXT_VIEW_H_
#define _ACKNEXT_VIEW_H_

#include "config.h"
#include "math.h"

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

#endif // _ACKNEXT_VIEW_H_
