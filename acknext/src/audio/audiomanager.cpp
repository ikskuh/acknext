#include "audiomanager.hpp"
#include "sound.hpp"

static SOUND * beepSound;

void AudioManager::initialize()
{
	// Create an option for this!
	if(Mix_Init(MIX_INIT_OGG) != MIX_INIT_OGG) {
		engine_seterror(ERR_SDL, "%s", Mix_GetError());
		return;
	}

	if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0) {
		engine_seterror(ERR_SDL, "%s", Mix_GetError());
		return;
	}

	beepSound = snd_load("/builtin/beep.wav");
}

void AudioManager::shutdown()
{
	snd_remove(beepSound);
	Mix_CloseAudio();
	Mix_Quit();
}

ACKNEXT_API_BLOCK
{
	void beep()
	{
		snd_play(beepSound, 100);
		SDL_Delay(200); // Beep pauses the game a bit
	}
}
