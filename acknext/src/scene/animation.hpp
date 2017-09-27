#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include <engine.hpp>

class Channel : public EngineObject<CHANNEL>
{
public:
	Channel(int frameCount);
	NOCOPY(Channel);
	~Channel();
};

class Animation : public EngineObject<ANIMATION>
{
public:
	Animation(char const * name, int channels);
	NOCOPY(Animation);
	~Animation();
};

#endif // ANIMATION_HPP
