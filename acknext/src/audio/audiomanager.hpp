#ifndef AUDIOMANAGER_HPP
#define AUDIOMANAGER_HPP

#include <engine.hpp>
#include <SDL2/SDL_mixer.h>

class AudioManager
{
public:
	AudioManager() = delete;

	static void initialize();

	// static void update();

	static void shutdown();
};

#endif // AUDIOMANAGER_HPP
