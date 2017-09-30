#include "framebuffer.hpp"

FrameBuffer::FrameBuffer()
{
	glCreateFramebuffers(1, &api().object);
}

FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &api().object);
}

ACKNEXT_API_BLOCK
{
	FRAMEBUFFER * framebuf_create()
	{
		return demote(new FrameBuffer());
	}

	// resize all textures attached
	void framebuf_resize(FRAMEBUFFER * fb, SIZE size)
	{
		ARG_NOTNULL(fb,);
		for(int i = 0; i < ACKNEXT_MAX_FRAMEBUFFER_TARGETS; i++)
		{
			if(fb->targets[i])
				bmap_set(fb->targets[i], size.width, size.height, GL_NONE, GL_NONE, nullptr);
		}
		if(fb->depthBuffer)
			bmap_set(fb->depthBuffer, size.width, size.height, GL_NONE, GL_NONE, nullptr);
	}

	void framebuf_update(FRAMEBUFFER * fb)
	{
		ARG_NOTNULL(fb,);

		for(int i = 0; i < ACKNEXT_MAX_FRAMEBUFFER_TARGETS; i++)
		{
			glNamedFramebufferTexture(
				fb->object,
				GL_COLOR_ATTACHMENT0 + i,
				(fb->targets[i] != nullptr) ? fb->targets[i]->object : 0,
				0);
		}

		glNamedFramebufferTexture(
			fb->object,
			GL_DEPTH_STENCIL_ATTACHMENT,
			(fb->depthBuffer != nullptr) ? fb->depthBuffer->object : 0,
			0);

		// if(framebuf_checkValid(fb) == false)
		// 	engine_seterror(ERR_INVALIDOPERATION, "framebuf_update(): The framebuffer is not valid!");
	}

	bool framebuf_checkValid(FRAMEBUFFER * fb)
	{
		ARG_NOTNULL(fb, false);
		auto status = glCheckNamedFramebufferStatus(fb->object, GL_DRAW_FRAMEBUFFER);
		return (status == GL_FRAMEBUFFER_COMPLETE);
	}

	void framebuf_remove(FRAMEBUFFER * fb)
	{
		FrameBuffer * buf = promote<FrameBuffer>(fb);
		ARG_NOTNULL(fb,);
		delete buf;
	}
}
