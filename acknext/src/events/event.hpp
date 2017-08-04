#ifndef EVENT_HPP
#define EVENT_HPP

#include <engine.hpp>
#include <stdarg.h>
#include <set>

class Event : public EngineObject<EVENT>
{
public:
	const bool userDefined;
private:
	std::set<EVENTHANDLER> handlers;
public:
	explicit Event(bool isUserDefined = false);
	NOCOPY(Event);
	~Event() = default;

	void clear();
	void attach(EVENTHANDLER ptr);
	void detach(EVENTHANDLER ptr);

	bool isEmpty() const {
		return this->handlers.empty();
	}

	void invoke(void * arg);
};

#endif // EVENT_HPP
