

#include <acknext.h>
#include "../include/acknext/ext/scheduler.h"

#include <vector>
#include <algorithm>
#include <stdlib.h>

#include <coroutine.h>

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

class Task
{
public:
	TASK api;
	bool shutdown;
	int id;
	bool success;
public:
	Task(ENTRYPOINT function, void * context);
	Task(Task const &) = delete;
	Task(Task &&) = delete;
	~Task();

	void updateStatus();

	bool alive() const;

	bool enabled() const {
		return !!(api.mask & tasks_enabled);
	}

	int status() const;

	void resume();
private:
	static void Trampoline(struct schedule *, void * ud);
};

static_assert(offsetof(Task,api) == 0);

class CoError
{
public:
	static const int kill = 1;
	static const int exception = 2;
	static const int exit = 3;
private:
	int id;
public:
	CoError(int num) : id(num) { }

	int reason() const { return id; }
};

static std::vector<Task*> tasks;
static struct schedule * schedule;
static Task * current = nullptr;

extern "C" void scheduler_init()
{
	schedule = coroutine_open();
}

extern "C" void scheduler_update()
{
	for(size_t i = 0; i < tasks.size();)
	{
		Task * co = tasks.at(i);
		if((co->alive() == false) || co->shutdown) {
			delete co;
			tasks.erase(tasks.begin() + i);
		} else {
			i += 1;
		}
	}

	// Sort for priority.
	std::sort(
		tasks.begin(),
        tasks.end(),
		[](Task * a, Task * b) {
			return a->api.priority < b->api.priority;
		});

	// Schedule everything
	for(Task * co : tasks)
	{
		if(co->shutdown) {
			continue;
		}
		// Skip all masked tasks
		if(!co->enabled()) {
			co->updateStatus();
			continue;
		}
		switch(co->status())
		{
			case COROUTINE_RUNNING:
				engine_log("Somehow scheduler_update got called "
						   "from within a coroutine!");
				continue;
			case COROUTINE_DEAD:
				// this should never happen!
				continue;
			case COROUTINE_READY:
			case COROUTINE_SUSPEND:
			{
				co->resume();
				break;
			}
			default:
				engine_log(
					"Coroutine %d is in unknown status %d!",
					co->id,
					co->status());
				break;
		}
	}
}

extern "C" void scheduler_shutdown()
{
	coroutine_close(schedule);
}

extern "C"
{
	BITFIELD tasks_enabled = BITFIELD_ALL;

	TASK * task_current;

	TASK * task_start(ENTRYPOINT function, void * context)
	{
		if(function == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "function must not be NULL!");
			return nullptr;
		}

		Task * task = new Task(function, context);
	    tasks.emplace_back(task);
		task->resume();

		return (TASK*)task;
	}

	TASK * task_defer(ENTRYPOINT function, void * context)
	{
		if(function == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "function must not be NULL!");
			return nullptr;
		}

		Task * task = new Task(function, context);
	    tasks.emplace_back(task);
		return (TASK*)task;
	}

	void task_exit()
	{
		if(::current == nullptr) {
			engine_seterror(ERR_INVALIDOPERATION, "task_exit() failed because no task is running!");
			abort();
		}
		// kill ourselves
		throw CoError(CoError::exit);
	}

	void task_kill(TASK * _task)
	{
		Task * task = (Task*)_task;
		if(task == nullptr) {
			task = ::current;
		}
		if(::current == task) {
			// kill ourselves
			throw CoError(CoError::kill);
		} else {
			task->shutdown = true;
		}
	}

	void task_yield()
	{
	    if(::current == nullptr) {
	        // Nothing to yield
	        engine_log("Call to task_yield() outside a coroutine!");
	        return;
	    }

		coroutine_yield(::schedule);

	    if(::current == nullptr)
	    {
	        engine_log("No current coroutine after yield. This is evil!");
	        throw CoError { 2 };
	    }

	    if(::current->shutdown)
	    {
	        throw CoError { 1 };
	    }
	}

}

Task::Task(ENTRYPOINT function, void *context) :
    shutdown(false),
    id(coroutine_new(::schedule, &Task::Trampoline, this)),
    success(false)
{
	api.function = function;
	api.context = context;
	api.state = TASK_READY;
	api.mask = BITFIELD_ALL;
	api.failed = event_create();
	api.finished = event_create();

	this->updateStatus();
}

Task::~Task()
{
	event_remove(api.failed);
	event_remove(api.finished);
}

bool Task::alive() const
{
	return (this->status() != COROUTINE_DEAD);
}

int Task::status() const
{
	return coroutine_status(::schedule, this->id);
}

void Task::resume()
{
	if(::current != nullptr) {
		engine_log("Somehow a coroutine was resumed in a non-valid context!");
		abort();
	}
	this->updateStatus();
	::current = this;
	task_current = (TASK*)this;
    coroutine_resume(::schedule, this->id);
	task_current = nullptr;
	::current = nullptr;
	this->updateStatus();

	if(this->alive() == false)
	{
		if(this->success)
		{
			event_invoke(this->api.finished, this->api.context);
		}
		else
		{
			event_invoke(this->api.failed, this->api.context);
		}
	}
}

void Task::updateStatus()
{
	if(this->enabled() == false) {
		api.state = TASK_DISABLED;
		return;
	}
	switch(this->status())
	{
		case COROUTINE_DEAD:
			api.state = TASK_DEAD;
			break;
		case COROUTINE_READY:
			api.state = TASK_READY;
			break;
		case COROUTINE_SUSPEND:
			api.state = TASK_SUSPENDED;
			break;
		case COROUTINE_RUNNING:
			api.state = TASK_RUNNING;
			break;
		default:
			engine_log("Invalid task state!");
			abort();
	}
}

void Task::Trampoline(struct schedule *, void * ud)
{
	Task * co = reinterpret_cast<Task*>(ud);
	try
	{
		co->api.function(co->api.context);
		co->success = true;
	}
	catch(int i)
	{
		engine_log("received: %d", i);
	}
	catch(const CoError & err)
	{
		switch(err.reason()) {
			case CoError::kill:
				co->success = false;
				break;
			case CoError::exit:
				co->success = true;
				break;
			default:
				// This is a wanted shutdown!
				engine_log("Coroutine Shutdown: %d", err.reason());
				break;
		}
	}
	catch(const std::exception & except)
	{
		engine_log("Coroutine exited unexpectedly: %s", except.what());
	}
	catch(...)
	{
		engine_log("Coroutine exited with unknown exception!");
	}
}
