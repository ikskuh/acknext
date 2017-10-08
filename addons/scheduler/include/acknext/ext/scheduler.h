#ifndef _ACKNEXT_EXT_SCHEDULER_H_
#define _ACKNEXT_EXT_SCHEDULER_H_

#include <acknext.h>

#ifdef _ACKNEXT_EXT_SCHEDULER_INTERNAL_
#define SCHEDCONST
#else
#define SCHEDCONST const
#endif

typedef void (*ENTRYPOINT)(void * context);

typedef struct TASK
{
	ENTRYPOINT SCHEDCONST function;
	void * SCHEDCONST context;
	var priority;
	BITFIELD mask;
	TASKSTATE state;

	EVENT * SCHEDCONST failed;
	EVENT * SCHEDCONST finished;
} TASK;

ACKFUN void scheduler_init();

ACKFUN void scheduler_update();

ACKFUN void scheduler_shutdown();

ACKFUN TASK * task_start(ENTRYPOINT function, void * context);

ACKFUN TASK * task_defer(ENTRYPOINT function, void * context);

ACKFUN void task_exit(); // gamestudio can't handle the swag

ACKFUN void task_kill(TASK * task); // at all!

ACKFUN void task_yield(); // wait(1)

ACKVAR BITFIELD tasks_enabled;

ACKVAR TASK * SCHEDCONST task_current;

#endif // _ACKNEXT_EXT_SCHEDULER_H_
