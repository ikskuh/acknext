#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

#include <engine.hpp>

class FrameBuffer :
	public EngineObject<FRAMEBUFFER>
{
public:
	FrameBuffer();
	NOCOPY(FrameBuffer);
	~FrameBuffer();
};

#endif // FRAMEBUFFER_HPP
