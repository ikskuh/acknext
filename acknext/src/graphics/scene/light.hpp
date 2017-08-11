#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <engine.hpp>

class Light : public EngineObject<LIGHT>
{
public:
	static Light * first;
	static Light * last;
	Light * previous;
	Light * next;
public:
	explicit Light(LIGHTTYPE type);
	NOCOPY(Light);
	~Light();
};

#endif // LIGHT_HPP
