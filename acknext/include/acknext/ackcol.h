#ifndef _ACKNEXT_ACKCOL_H_
#define _ACKNEXT_ACKCOL_H_

#include "config.h"
#include "ackmath.h"
#include "ackdef.h"
#include "scene.h"
#include "ackentity.h"

typedef struct
{
	HULLTYPE ACKCONST type;
	ENTITY * ACKCONST entity;
	void * userData;
} HULL;

typedef struct
{
	VECTOR contact, normal;
	HULL * hull;
	var distance;
} COLLISION;

// Hull API:
ACKFUN HULL * hull_createSphere(ENTITY * ent, float radius);

ACKFUN HULL * hull_createBox(ENTITY * ent, VECTOR const * size);

ACKFUN void hull_remove(HULL * hull);

// Collision API:
ACKFUN COLLISION * c_trace(
	VECTOR const * from,
	VECTOR const * to,
	BITFIELD mask);

ACKFUN COLLISION * c_move(
	ENTITY * ent,
	VECTOR const * from,
	VECTOR const * to,
	BITFIELD mask,
	MOVEFLAGS flags);

ACKFUN COLLISION * c_warp(
	ENTITY * ent,
	VECTOR const * position,
	QUATERNION const * rotation,
	BITFIELD mask,
	WARPFLAGS flags);

#endif // _ACKNEXT_ACKCOL_H_
