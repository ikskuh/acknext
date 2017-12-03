#include "animation.hpp"
#include <assert.h>

Channel::Channel(int frameCount)
{
	assert(frameCount > 0);
	api().frameCount = frameCount;
	api().frames = new FRAME[api().frameCount];
}

Channel::~Channel()
{
	delete api().frames;
}

Animation::Animation(char const * name, int channels)
{
	assert(name != nullptr);
	assert(channels > 0);
	api().channelCount = channels;
	strcpy(api().name, name);
	api().flags = 0;
	api().channels = new CHANNEL*[channels];
	for(int i = 0; i < channels; i++)
		api().channels[i] = nullptr;
}

Animation::~Animation()
{
	delete api().channels;
}

ACKNEXT_API_BLOCK
{
	CHANNEL * chan_create(int frames)
	{
		if(frames <= 0) {
			engine_seterror(ERR_INVALIDARGUMENT, "A channel must have at least a single frame");
			return nullptr;
		}
		return demote(new Channel(frames));
	}

	void chan_remove(CHANNEL * chan)
	{
		ARG_NOTNULL(chan,);
		delete promote<Channel>(chan);
	}

	ANIMATION * anim_create(char const * name, int channels)
	{
		ARG_NOTNULL(name,nullptr);
		if(channels <= 0) {
			engine_seterror(ERR_INVALIDARGUMENT, "Animation must have at least a single channel.");
			return nullptr;
		}
		int len = strlen(name);
		if(len <= 0 || len >= 256) {
			engine_seterror(ERR_INVALIDARGUMENT, "Animation name must have at least a single char and must not be longer than 255 chars.");
			return nullptr;
		}
		return demote(new Animation(name, channels));
	}

	void anim_remove(ANIMATION * anim)
	{
		ARG_NOTNULL(anim,);
		delete promote<Animation>(anim);
	}
}
