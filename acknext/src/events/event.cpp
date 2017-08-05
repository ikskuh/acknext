#include "event.hpp"

Event::Event(bool isUserDefined) :
    EngineObject<EVENT>(),
    userDefined(isUserDefined),
    handlers()
{

}

void Event::clear()
{
	this->handlers.clear();
}

void Event::attach(EVENTHANDLER ptr)
{
	this->handlers.emplace(ptr);
}

void Event::detach(EVENTHANDLER ptr)
{
	auto foo = this->handlers.find(ptr);
	if(foo != this->handlers.end()) {
		this->handlers.erase(foo);
	}
}

void Event::invoke(void * arg)
{
	for(EVENTHANDLER const & ev : this->handlers) {
#ifdef ACKNEXT_HAS_SCHEDULER
		task_start(ev, arg);
#else
		ev(arg);
#endif
	}
}

ACKNEXT_API_BLOCK
{
	EVENT * event_create()
	{
		return demote(new Event(true));
	}

	void event_attach(EVENT * _ev, EVENTHANDLER handler)
	{
		Event * ev = promote<Event>(_ev);
		if(ev && handler) {
			ev->attach(handler);
		}
	}

	void event_detach(EVENT * _ev, EVENTHANDLER handler)
	{
		Event * ev = promote<Event>(_ev);
		if(ev && handler) {
			ev->detach(handler);
		}
	}

	void event_clear(EVENT * _ev)
	{
		Event * ev = promote<Event>(_ev);
		if(ev) {
			ev->clear();
		}
	}

	void event_invoke(EVENT * _ev, void * arg)
	{
		Event * ev = promote<Event>(_ev);
		if(ev) {
			ev->invoke(arg);
		}
	}

	void event_remove(EVENT * _ev)
	{
		Event * ev = promote<Event>(_ev);
		if(ev && ev->userDefined) {
			delete ev;
		}
	}
}
