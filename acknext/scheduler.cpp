#include "engine.h"
#include <memory>
#include <list>

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

    Coroutine(EntryPoint ep, void * context) :
        ep(ep),
        context(context),
        id(coroutine_new(::schedule, &Coroutine::Trampoline, this))
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
        catch(const CoError & err)
        {
            // This is a wanted shutdown!
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
    for(Coroutine & co : coroutines)
    {
        switch(co.status())
        {
            case COROUTINE_RUNNING:
                engine_log("Somehow scheduler_update got called "
                           "from within a coroutine!");
                continue;
            case COROUTINE_DEAD:
                continue;
            case COROUTINE_READY:
            case COROUTINE_SUSPEND:
            {
                ::current = &co;
                context = co.context;

                co.resume();

                context = nullptr;
                ::current = nullptr;
                break;
            }
            default:
                engine_log("Coroutine %d is in unknown status %d!", co.id, co.status());
                break;
        }
    }
}




ACKFUN void sched_start(Coroutine::EntryPoint ep, void * context)
{
    coroutines.emplace_back(ep, context);
}

ACKFUN void sched_wait()
{
    if(::current == nullptr) {
        // Nothing to yield
        engine_log("Call to sched_wait() outside a coroutine!");
        return;
    }

    coroutine_yield(schedule);
}
