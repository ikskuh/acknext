#ifndef _ACKNEXT_SCHEDULER_H_
#define _ACKNEXT_SCHEDULER_H_

#include "config.h"
#include "ackenum.h"
#include "ackmath.h"
#include "event.h"

typedef void (*ENTRYPOINT)(void * context);

typedef struct
{
	ENTRYPOINT ACKCONST function;
	void * ACKCONST context;
	var priority;
	BITFIELD mask;
	TASKSTATE state;

	EVENT * ACKCONST failed;
	EVENT * ACKCONST finished;
} TASK;

ACKFUN TASK * task_start(ENTRYPOINT function, void * context);

ACKFUN TASK * task_defer(ENTRYPOINT function, void * context);

ACKFUN void task_kill(TASK * task);

ACKFUN void task_yield(); // wait(1)

ACKVAR BITFIELD tasks_enabled;

ACKVAR TASK * ACKCONST task_current;

#endif // _ACKNEXT_SCHEDULER_H_
