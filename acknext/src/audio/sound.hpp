#ifndef SOUND_HPP
#define SOUND_HPP

#include <engine.hpp>
#include <SDL2/SDL_mixer.h>

class Sound : public EngineObject<SOUND>
{
public:
	Mix_Chunk * const chunk;
public:
	Sound(Mix_Chunk * chunk);
	NOCOPY(Sound);
	~Sound();
};

#endif // SOUND_HPP
