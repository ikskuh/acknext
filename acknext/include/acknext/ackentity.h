#ifndef _ACKNEXT_ACKENTITY_H_
#define _ACKNEXT_ACKENTITY_H_

#include "config.h"
#include "ackmath.h"
#include "scene.h"
#include "scheduler.h"

typedef struct
{
	VECTOR position;
	QUATERNION rotation;
	VECTOR scale;
	MODEL * model;
	MATERIAL * material;

	EVENT * removed;
	ENTITYFLAGS flags;
} ENTITY;

ACKFUN ENTITY * ent_create(
	char const * fileName,
	VECTOR * const position,
	ENTRYPOINT action);

ACKFUN void ent_remove(ENTITY * ent);

ACKFUN ENTITY * ent_next(ENTITY const * ent);

#endif // _ACKNEXT_ACKENTITY_H_
