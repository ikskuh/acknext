#include "bitmap.hpp"

#include <SDL2/SDL_image.h>

static GLenum getTextureTarget(TEXTURETYPE type)
{
	switch(type) {
		case TEX_1D: return GL_TEXTURE_1D;
		case TEX_2D: return GL_TEXTURE_2D;
		case TEX_3D: return GL_TEXTURE_3D;
		case TEX_1D_ARRAY: return GL_TEXTURE_1D_ARRAY;
		case TEX_2D_ARRAY: return GL_TEXTURE_2D_ARRAY;
		default:
			engine_seterror(ERR_INVALIDARGUMENT, "The given texture type is not valid!");
			return GL_INVALID_ENUM;
	}
}

static GLenum getTextureFormat(PIXELFORMAT format)
{
	switch(format) {
		case FORMAT_RGB8: return GL_RGB8;
		case FORMAT_RGBA8: return GL_RGBA8;
		case FORMAT_BGRA8: return GL_RGBA8;
		case FORMAT_RGBA16F: return GL_RGBA16F;
		case FORMAT_RGBA32F: return GL_RGBA32F;
		default:
			engine_seterror(ERR_INVALIDARGUMENT, "The given format is not supported!");
			return GL_INVALID_ENUM;
	}
}

Bitmap::Bitmap(TEXTURETYPE type)
{
	api().type = type;
	this->target = getTextureTarget(type);
	glCreateTextures(this->target, 1, &this->id);
}

Bitmap::~Bitmap()
{
	glDeleteTextures(1, &this->id);
}

ACKNEXT_API_BLOCK
{
	BITMAP * bmap_create(TEXTURETYPE type)
	{
		return demote(new Bitmap(type));
	}

	BITMAP * bmap_createblack(int width, int height, PIXELFORMAT format)
	{
		if(width < 0 || height < 0) {
			engine_seterror(ERR_INVALIDARGUMENT, "width and height must be greater 0!");
			return nullptr;
		}
		BITMAP * bmp = bmap_create(TEX_2D);
		bmap_set(bmp, width, height, format, nullptr);
		return bmp;
	}

	BITMAP * bmap_load(char const * fileName)
	{
		SDL_RWops *rwops = PHYSFSRWOPS_openRead(fileName);
		if(rwops == nullptr) {
			engine_setsdlerror();
			return nullptr;
		}

		SDL_Surface * surface = IMG_Load_RW(rwops, 1);
		if(surface == nullptr) {
			engine_setsdlerror();
			return nullptr;
		}
		SDL_Surface * converted = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ABGR8888, 0);
		if(converted == nullptr) {
			engine_setsdlerror();
			SDL_FreeSurface(surface);
			return nullptr;
		}
		surface = converted;

		BITMAP * bmp = bmap_create(TEX_2D);

		SDL_LockSurface(surface);
		bmap_set(bmp, surface->w, surface->h, FORMAT_RGBA8, surface->pixels);
		SDL_UnlockSurface(surface);

		SDL_FreeSurface(surface);
		return bmp;
	}

	void bmap_set(BITMAP * bitmap, int width, int height, PIXELFORMAT format, void const * data)
	{
		Bitmap * bmp = promote<Bitmap>(bitmap);
		if(bmp == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "bitmap must not be NULl!");
			return;
		}
		if(bmp->target != GL_TEXTURE_1D_ARRAY && bmp->target != GL_TEXTURE_2D) {
			engine_seterror(ERR_INVALIDOPERATION, "bitmap must be either TEX_1D_ARRAY or TEX_2D!");
			return;
		}
		if(width < 0 || height < 0) {
			engine_seterror(ERR_INVALIDARGUMENT, "width and height must be greater 0!");
			return;
		}
		GLenum fmt = getTextureFormat(format);
		if(fmt == GL_INVALID_ENUM) {
			engine_seterror(ERR_INVALIDARGUMENT, "format must be a valid texture format!");
			return;
		}

		GLenum dfmt = GL_INVALID_ENUM;
		GLenum pfmt = GL_INVALID_ENUM;
		switch(format) {
			case FORMAT_RGB8:
			case FORMAT_RGBA8:
				dfmt = GL_UNSIGNED_BYTE;
				pfmt = GL_RGBA;
				break;
			case FORMAT_BGRA8:
				dfmt = GL_UNSIGNED_BYTE;
				pfmt = GL_BGRA;
				break;
			case FORMAT_RGBA16F: pfmt = GL_RGBA; dfmt = GL_HALF_FLOAT; break;
			case FORMAT_RGBA32F: pfmt = GL_RGBA; dfmt = GL_FLOAT; break;
			default: abort();
		}

		glTextureStorage2D(
			bmp->id,
			1,
			fmt,
			width,
			height);

		if(data != nullptr) {
			glTextureSubImage2D(
				bmp->id,
				0,
				0, 0,
				width, height,
				pfmt,
				dfmt,
				data);
		}

		glTextureParameteri(bmp->id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(bmp->id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(bmp->id, GL_TEXTURE_WRAP_R, GL_REPEAT);
		glTextureParameteri(bmp->id, GL_TEXTURE_WRAP_S, GL_REPEAT);

		bitmap->width = width;
		bitmap->height = height;
		bitmap->format = format;
		bitmap->depth = 1;
	}

	void bmap_remove(BITMAP * bitmap)
	{
		Bitmap * bmp = promote<Bitmap>(bitmap);
		if(bmp) {
			delete bmp;
		}
	}
}
