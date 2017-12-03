#ifndef _ACKNEXT_ACKSOUND_H_
#define _ACKNEXT_ACKSOUND_H_

#include "config.h"
#include "ackmath.h"

typedef struct
{
	var duration;
} SOUND;

typedef uint32_t SOUNDHANDLE;

ACKFUN SOUND * snd_load(char const * fileName);

ACKFUN void snd_remove(SOUND * snd);

ACKFUN SOUNDHANDLE snd_play(SOUND * sound, var volume);

ACKFUN SOUNDHANDLE snd_loop(SOUND * sound, var volume);

ACKFUN void snd_pause(SOUNDHANDLE handle);
ACKFUN void snd_resume(SOUNDHANDLE handle);
ACKFUN void snd_stop(SOUNDHANDLE handle);

ACKFUN bool snd_playing(SOUNDHANDLE handle);

#endif // _ACKNEXT_ACKSOUND_H_
