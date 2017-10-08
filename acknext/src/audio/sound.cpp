#include "sound.hpp"

Sound::Sound(Mix_Chunk * chunk) :
    EngineObject<SOUND>(),
    chunk(chunk)
{
	// TODO: Calculate the sound duration here!
	api().duration = -1;
}

Sound::~Sound()
{
	Mix_FreeChunk(this->chunk);
}

ACKNEXT_API_BLOCK
{
	SOUND * snd_load(char const * fileName)
	{
		// TODO: Reimplement
		return nullptr;

		ARG_NOTNULL(fileName, nullptr);

		ACKFILE * file = file_open_read(fileName);
		if(!file) {
			engine_seterror(ERR_FILESYSTEM, "snd_load: file not found: %s", fileName);
			return nullptr;
		}

		SDL_RWops *rwops = SDL_AllocRW();
		{
			assert(rwops);
			rwops->size = [](SDL_RWops *c) -> Sint64 {
				return file_size((ACKFILE*)c->hidden.unknown.data1);
			};
			rwops->seek = [](SDL_RWops *c,Sint64 offset,int whence) -> Sint64 {
				auto * f = (ACKFILE*)c->hidden.unknown.data1;
				if(whence == RW_SEEK_CUR) {
					offset = file_tell(f) + offset;
				} else if(whence == RW_SEEK_END) {
					auto len = file_size(f);
					if(len < 0) return SDL_SetError("Can't seek with _END in a non-sized stream!");
					offset = len - offset;
				}
				file_seek(f, offset);
				return offset;
			};
			rwops->read = [](SDL_RWops *c, void *ptr, size_t size, size_t num) -> size_t {
				return file_read((ACKFILE*)c->hidden.unknown.data1, ptr, size * num);
			};
			rwops->write = [](SDL_RWops *c, const void *ptr, size_t size, size_t num) -> size_t {
				return file_write((ACKFILE*)c->hidden.unknown.data1, ptr, size * num);
			};
			rwops->close= [](SDL_RWops *context) -> int {
				file_close((ACKFILE*)context->hidden.unknown.data1);
				return 0;
			};
			rwops->type = 0xdeadbeef;
			rwops->hidden.unknown.data1 = file;
		}

		if(rwops == nullptr) {
			engine_setsdlerror();
			return nullptr;
		}

		Mix_Chunk *chunk = Mix_LoadWAV_RW(rwops, 1);
		if(chunk == nullptr) {
			engine_seterror(ERR_SDL, "%s", Mix_GetError());
			return nullptr;
		}
		return demote(new Sound(chunk));
	}

	void snd_remove(SOUND * snd)
	{
		Sound * sound = promote<Sound>(snd);
		if(sound) {
			delete sound;
		}
	}

	SOUNDHANDLE snd_play(SOUND * _sound, var volume)
	{
		Sound * sound = promote<Sound>(_sound);
		if(sound == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "sound must not be NULL!");
			return -1;
		}
		int chan = Mix_PlayChannel(-1, sound->chunk, 0);
		Mix_Volume(chan, int(0.01 * clamp(volume, 0, 100) * MIX_MAX_VOLUME));
		// TODO: Mix_Volume(chan, 0.01 * volume * MIX_MAX_VOLUME);
		return chan;
	}

	SOUNDHANDLE snd_loop(SOUND * _sound, var volume)
	{
		Sound * sound = promote<Sound>(_sound);
		if(sound == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "sound must not be NULL!");
			return 0;
		}
		int chan = Mix_PlayChannel(-1, sound->chunk, -1);
		Mix_Volume(chan, 0.01 * volume * MIX_MAX_VOLUME);
		return chan;
	}

	void snd_pause(SOUNDHANDLE handle)
	{
		Mix_Pause(handle);
	}

	void snd_resume(SOUNDHANDLE handle)
	{
		Mix_Resume(handle);
	}

	void snd_stop(SOUNDHANDLE handle)
	{
		Mix_HaltChannel(handle);
	}

	bool snd_playing(SOUNDHANDLE handle)
	{
		return Mix_Playing(handle);
	}
}
