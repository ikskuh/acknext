#include "engine.h"
#include "bitmap-detail.h"

static GLenum translate(enum PIXELFORMAT format)
{
	switch(format) {
		case RGBA8: return GL_RGBA8;
		case RGBA16F: return GL_RGBA16F;
		case RGBA32F: return GL_RGBA32F;
		default:
			engine_seterror(INVALID_ARGUMENT, "The given format is not supported!");
			return GL_INVALID_ENUM;
	}
}

BITMAPdetail::BITMAPdetail(BITMAP * bitmap) : bitmap(bitmap), id(0), format(GL_INVALID_ENUM)
{
	glCreateTextures(GL_TEXTURE_2D, 1, &this->id);
}

BITMAPdetail::~BITMAPdetail()
{
	glDeleteTextures(1, &this->id);
}

ACKFUN BITMAP * bmap_create()
{
	BITMAP * bmp = new BITMAP;
	bmp->format = (enum PIXELFORMAT)0;
	bmp->width = 0;
	bmp->height = 0;
	bmp->_detail = new BITMAPdetail(bmp);
	return bmp;
}

ACKFUN BITMAP * bmap_createblack(int width, int height, enum PIXELFORMAT format)
{
	BITMAP * bmp = bmap_create();
	bmap_set(bmp, width, height, format, nullptr);
	return bmp;
}

ACKFUN void bmap_remove(BITMAP * bitmap)
{
	if(bitmap == nullptr) {
		return;
	}
	delete bitmap->_detail;
	delete bitmap;
}

ACKFUN void bmap_set(BITMAP * bitmap, int width, int height, enum PIXELFORMAT format, void const * data)
{
	if(bitmap == nullptr) {
		engine_seterror(INVALID_ARGUMENT, "Cannot set data of NULL bitmap!");
		return;
	}

	if(width <= 0 || height <= 0) {
		engine_seterror(INVALID_ARGUMENT, "width and height must be larger than zero!");
		return;
	}
	GLenum fmt = translate(format);
	if(fmt == GL_INVALID_ENUM) {
		return;
	}

	GLenum dfmt;
	switch(format) {
		case RGBA8: dfmt = GL_UNSIGNED_BYTE; break;
		case RGBA16F: dfmt = GL_HALF_FLOAT; break;
		case RGBA32F: dfmt = GL_FLOAT; break;
		default: abort();
	}

	glTextureStorage2D(
		bitmap->_detail->id,
		1,
		fmt,
		width,
		height);

	if(data != nullptr) {
		glTextureSubImage2D(
			bitmap->_detail->id,
			0,
			0, 0,
			width, height,
			GL_RGBA,
			dfmt,
			data);
	}

	glTextureParameteri(bitmap->_detail->id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(bitmap->_detail->id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(bitmap->_detail->id, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTextureParameteri(bitmap->_detail->id, GL_TEXTURE_WRAP_S, GL_REPEAT);

	bitmap->width = width;
	bitmap->height = height;
	bitmap->format = format;
}
