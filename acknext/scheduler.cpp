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

struct Coroutine
{
    typedef void (*EntryPoint)();

    EntryPoint ep;
    void * context;
    int const id;
    bool shutdown;
    var priority;
    HANDLE const handle;

    Coroutine(EntryPoint ep, void * context) :
        ep(ep),
        context(context),
        id(coroutine_new(::schedule, &Coroutine::Trampoline, this)),
        shutdown(false),
        priority(task_priority), // copy the current priority
        handle(handle_getnew(HANDLE_TASK))
    {

    }

    Coroutine(Coroutine const & other) = delete;
    Coroutine(Coroutine && other) = delete;

    ~Coroutine()
    {

    }

    int status()
    {
        return coroutine_status(::schedule, this->id);
    }

    void resume()
    {
        coroutine_resume(::schedule, this->id);
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
    return lhs.priority < rhs.priority;
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
        return co.status() == COROUTINE_DEAD;
    });

    // Sort for priority.
    coroutines.sort();

    // Schedule everything
    for(Coroutine & co : coroutines)
    {
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
                context = co.context;
                task_priority = co.priority;

                co.resume();

                // Same goes here
                co.priority = task_priority;
                context = nullptr;
                ::current = nullptr;
                break;
            }
            default:
                engine_log("Coroutine %d is in unknown status %d!", co.id, co.status());
                break;
        }
    }

    task_priority = 0.0;
}




ACKFUN HANDLE task_start(Coroutine::EntryPoint ep, void * context)
{
    coroutines.emplace_back(ep, context);
    return coroutines.back().handle;
}

ACKFUN void task_kill(HANDLE htask)
{
    auto cofind = std::find_if(
        coroutines.begin(),
        coroutines.end(),
        [htask](const Coroutine & co)
        {
            return co.handle == htask;
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
        engine_log("Call to sched_wait() outside a coroutine!");
        return;
    }
    coroutine_yield(schedule);

    if(::current == nullptr)
    {
        engine_log("No current coroutine after yield. This is evil!");
        throw CoError { 2 };
    }

    if(::current->shutdown)
    {
        throw 1;
        // throw CoError { 1 };
    }
}
