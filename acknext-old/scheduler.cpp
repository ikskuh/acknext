#include "engine.h"
#include <memory>
#include <list>
#include <algorithm>

#include "../coroutine/coroutine.h"

static struct schedule * schedule;

struct CoError
{
	int reason;
};

extern "C"
{
	struct memptr;
	extern struct memptr _taskdata_start;
	extern struct memptr _taskdata_end;
}

struct TaskGlobals
{
    const size_t TGSIZE;
    uint8_t * buffer;

    TaskGlobals() :
        TGSIZE(((uint8_t const *)&_taskdata_end - (uint8_t const *)&_taskdata_start))
    {
        buffer = new uint8_t[TGSIZE];
        memset(buffer, 0, TGSIZE);
    }

    TaskGlobals(TaskGlobals const &) = delete;
    TaskGlobals(TaskGlobals &&) = delete;

    ~TaskGlobals()
    {
        delete buffer;
    }

    void readback()
    {
        memcpy(buffer, &_taskdata_start, TGSIZE);
    }

    void assign()
    {
        memcpy(&_taskdata_start, buffer, TGSIZE);
    }

    template<typename T>
    void set(T const & var, T value)
    {
        size_t offset = (uint8_t const *)&var - (uint8_t const * )&_taskdata_start;
        *((T*)(&buffer[offset])) = value;
    }

    void set(void const * var, void const * value, size_t size)
    {
        size_t offset = (uint8_t const *)var - (uint8_t const * )&_taskdata_start;
        memcpy(&buffer[offset], value, size);
    }

    template<typename T>
    T get(T const & var) const
    {
        size_t offset = (uint8_t const *)&var - (uint8_t const * )&_taskdata_start;
        return *((T*)(&buffer[offset]));
    }

    template<typename T>
    T const & value(T const & var) const
    {
        size_t offset = (uint8_t const *)&var - (uint8_t const * )&_taskdata_start;
        return *((T*)(&buffer[offset]));
    }

    template<typename T>
    T & value(T const & var)
    {
        size_t offset = (uint8_t const *)&var - (uint8_t const * )&_taskdata_start;
        return *((T*)(&buffer[offset]));
    }
};

struct Coroutine
{
    void (*ep)();
    TaskGlobals globals;
    int const id;
    bool shutdown;
    HANDLE const handle;

    Coroutine(void (*ep)(), void * context) :
        ep(ep),
        globals(),
        id(coroutine_new(::schedule, &Coroutine::Trampoline, this)),
        shutdown(false),
        handle(handle_getnew(HANDLE_TASK))
    {
        globals.set(::task.context, context);
        globals.set(::task.mask, -1);
        globals.set(::task.priority, ::task.priority);
    }

    Coroutine(Coroutine const & other) = delete;
    Coroutine(Coroutine && other) = delete;

    ~Coroutine()
    {

    }

    bool alive()
    {
        return (this->status() != COROUTINE_DEAD);
    }

    int status()
    {
        return coroutine_status(::schedule, this->id);
    }

    void resume()
    {
        coroutine_resume(::schedule, this->id);
    }

    void yield()
    {
        coroutine_yield(::schedule);
    }

private:
    static void Trampoline(struct schedule *, void * ud)
    {
        Coroutine * co = reinterpret_cast<Coroutine*>(ud);
        try
        {
            co->ep();
        }
        catch(int i)
        {
            engine_log("received: %d", i);
        }
        catch(const CoError & err)
        {
            // This is a wanted shutdown!
            engine_log("Coroutine Shutdown: %d", err.reason);
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
};

// This is required for coroutine sorting!
bool operator < (const Coroutine & lhs, const Coroutine & rhs)
{
    return lhs.globals.get(task.priority) < rhs.globals.get(task.priority);
}

static Coroutine * current;

static std::list<Coroutine> coroutines;

void scheduler_initialize()
{
    engine_log("Initializing scheduler");
    schedule = coroutine_open();
}

void scheduler_shutdown()
{
    engine_log("Stopping scheduler");

    coroutines.clear();

    coroutine_close(schedule);
}

void scheduler_update()
{
    // Remove all dead coroutines.
    coroutines.remove_if([](Coroutine & co) {
        return (co.alive() == false) || co.shutdown;
    });

    // Sort for priority.
    coroutines.sort();

    // Schedule everything
    for(Coroutine & co : coroutines)
    {
        if(co.shutdown) {
            continue;
        }
        // Skip all masked tasks
        if((co.globals.value(::task).mask & task_enabled) == 0) {
            continue;
        }
        switch(co.status())
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
                // This is some stupid back and forth of variables,
                // but it makes the API easier.
                ::current = &co;

                co.globals.assign();
                co.resume();
                co.globals.readback();

                ::current = nullptr;
                break;
            }
            default:
                engine_log("Coroutine %d is in unknown status %d!", co.id, co.status());
                break;
        }
    }
}

void scheduler_setvar(TASK * task, void * ptr, void * data, size_t size)
{
	auto cofind = std::find_if(
        coroutines.begin(),
        coroutines.end(),
        [task](const Coroutine & co)
        {
            return (&co.globals.value(::task)) == task;
        });
    if(cofind == coroutines.end()) {
        return;
    }
    (*cofind).globals.set(ptr, data, size);
}

ACKFUN TASK * task_start(void (*ep)(), void * context)
{
    coroutines.emplace_back(ep, context);

    Coroutine & co = coroutines.back();

    return &co.globals.value(task);
}

ACKFUN void task_kill(TASK * task)
{
    auto cofind = std::find_if(
        coroutines.begin(),
        coroutines.end(),
        [task](const Coroutine & co)
        {
            return (&co.globals.value(::task)) == task;
        });
    if(cofind == coroutines.end()) {
        return;
    }
    (*cofind).shutdown = true;
}

ACKFUN void task_wait()
{
    if(::current == nullptr) {
        // Nothing to yield
        engine_log("Call to task_wait() outside a coroutine!");
        return;
    }

    ::current->yield();

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
