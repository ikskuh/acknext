#include "framebuffer.hpp"

#include "../core/glenum-translator.hpp"

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
		bool hasChanges = false;
		for(int i = 0; i < ACKNEXT_MAX_FRAMEBUFFER_TARGETS; i++)
		{
			BITMAP * bmp = fb->targets[i];
			if(bmp == nullptr)
				continue;
			if(bmp->width == size.width && bmp->height == size.height)
				continue;
			bmap_renew(bmp);
			bmap_set(bmp, size.width, size.height, GL_NONE, GL_NONE, nullptr);
			hasChanges = true;
		}
		if(fb->depthBuffer)
		{
			if(fb->depthBuffer->width != size.width
			   || fb->depthBuffer->height != size.height)
			{
				bmap_renew(fb->depthBuffer);
				bmap_set(fb->depthBuffer, size.width, size.height, GL_NONE, GL_NONE, nullptr);
				hasChanges = true;
			}
		}
		fb->size = size;

		if(hasChanges) {
			framebuf_update(fb);
		}
	}

	void framebuf_update(FRAMEBUFFER * fb)
	{
		ARG_NOTNULL(fb,);

		GLenum drawbuffers[ACKNEXT_MAX_FRAMEBUFFER_TARGETS];

		for(int i = 0; i < ACKNEXT_MAX_FRAMEBUFFER_TARGETS; i++)
		{
			glNamedFramebufferTexture(
				fb->object,
				GL_COLOR_ATTACHMENT0 + i,
				(fb->targets[i] != nullptr) ? fb->targets[i]->object : 0,
				0);
			drawbuffers[i] = (fb->targets[i] != nullptr) ? (GL_COLOR_ATTACHMENT0 + i) : GL_NONE;
		}

		glNamedFramebufferDrawBuffers(
			fb->object,
			ACKNEXT_MAX_FRAMEBUFFER_TARGETS,
			drawbuffers);

		glNamedFramebufferTexture(
			fb->object,
			GL_DEPTH_STENCIL_ATTACHMENT,
			(fb->depthBuffer != nullptr) ? fb->depthBuffer->object : 0,
			0);

		GLenum status = glCheckNamedFramebufferStatus(fb->object, GL_DRAW_FRAMEBUFFER);
		if(status != GL_FRAMEBUFFER_COMPLETE) {
			engine_log("framebuf_update() → %s", GLenumToString(status));
		}
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
