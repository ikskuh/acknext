#ifndef _ACKNEXT_ACKENTITY_H_
#define _ACKNEXT_ACKENTITY_H_

#include "config.h"
#include "ackmath.h"
#include "scene.h"
#include "scheduler.h"

typedef struct
{
	// Transform
	VECTOR position;
	QUATERNION rotation;
	VECTOR scale;

	// Visuals
	MODEL * model;
	MATERIAL * material;
	FRAME pose[ACKNEXT_MAX_BONES];

	ENTITYFLAGS flags;

	// Events
	EVENT * ACKCONST removed;
} ENTITY;

ACKFUN ENTITY * ent_create(
	char const * fileName,
	VECTOR * const position,
	ENTRYPOINT action);

// Resets the entities pose to the entities models default pose
ACKFUN void ent_posereset(ENTITY * ent);

// progress: time in seconds
ACKFUN void ent_animate(ENTITY * ent, char const * animation, double progress);

ACKFUN void ent_remove(ENTITY * ent);

ACKFUN ENTITY * ent_next(ENTITY const * ent);

#endif // _ACKNEXT_ACKENTITY_H_
