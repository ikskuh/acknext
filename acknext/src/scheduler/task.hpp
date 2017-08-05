#ifndef TASK_HPP
#define TASK_HPP

#include <engine.hpp>
#include "../coroutine/coroutine.h"

/*
 * Scheduler system:
 * - task_start executes until the first yield() or return, so
 *   initialization can be done inside the called task function
 * - task_defer starts execution after the next yield(),
 *   so initialization can be done on the returned TASK object
 * - Events use task_start to enable synchronous event handling
 * - Tasks are sorted at the start of a frame, new tasks will be appended
 *   to the current queue. When a task is started by task_start, it will
 *   not be executed a second time in the frame it was started
 * - Tasks have a set of task-local variables that reside in a
 *   special section of the engines core. User variables cannot be made
 *   task-local.
 */

class Task : public EngineObject<TASK>
{
public:
	bool shutdown;
	int id;
	bool success;
public:
	Task(ENTRYPOINT function, void * context);
	NOCOPY(Task);
	~Task();

	void updateStatus();

	bool alive() const;

	bool enabled() const {
		return !!(api().mask & tasks_enabled);
	}

	int status() const;

	void resume();
private:
	static void Trampoline(struct schedule *, void * ud);
};

#endif // TASK_HPP
