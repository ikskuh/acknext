#include "bitmap.hpp"

#include <SDL2/SDL_image.h>

Bitmap::Bitmap(GLenum type, GLenum format)
{
	api().target = type;
	api().format = format;
	glCreateTextures(api().target, 1, &api().object);
}

Bitmap::~Bitmap()
{
	glDeleteTextures(1, &api().object);
}

ACKNEXT_API_BLOCK
{
	// for loading bitmaps
	int bmap_miplevels = 0; // 0=infinite

	BITMAP * bmap_create(GLenum type, GLenum format)
	{
		return demote(new Bitmap(type, format));
	}

	BITMAP * bmap_createblack(int width, int height, GLenum format)
	{
		if(width < 0 || height < 0) {
			engine_seterror(ERR_INVALIDARGUMENT, "width and height must be greater 0!");
			return nullptr;
		}
		BITMAP * bmp = bmap_create(GL_TEXTURE_2D, format);
		bmap_set(bmp, width, height, format, GL_UNSIGNED_BYTE, nullptr);
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

		BITMAP * bmp = bmap_create(GL_TEXTURE_2D, GL_RGBA8);

		SDL_LockSurface(surface);
		bmap_set(bmp, surface->w, surface->h, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
		SDL_UnlockSurface(surface);

		SDL_FreeSurface(surface);
		return bmp;
	}

	void bmap_set(BITMAP * bitmap, int width, int height, GLenum pixelFormat, GLenum channelFormat, void const * data)
	{
		Bitmap * bmp = promote<Bitmap>(bitmap);
		if(bmp == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "bitmap must not be NULl!");
			return;
		}
		if(bitmap->target != GL_TEXTURE_1D_ARRAY && bitmap->target != GL_TEXTURE_2D) {
			engine_seterror(ERR_INVALIDOPERATION, "bitmap must be either GL_TEXTURE_1D_ARRAY or GL_TEXTURE_2D!");
			return;
		}
		if(width < 0 || height < 0) {
			engine_seterror(ERR_INVALIDARGUMENT, "width and height must be greater 0!");
			return;
		}

		int levels = 0;
		{
			int a = width, b = height;
			while(a > 0 && b > 0) {
				a >>= 1;
				b >>= 1;
				levels++;
			}
		}

		glTextureStorage2D(
			bitmap->object,
			levels,
			bitmap->format,
			width,
			height);

		if(data != nullptr) {
			glTextureSubImage2D(
				bitmap->object,
				0,
				0, 0,
				width, height,
				pixelFormat,
				channelFormat,
				data);
		}

		glTextureParameteri(bitmap->object, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(bitmap->object, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(bitmap->object, GL_TEXTURE_WRAP_R, GL_REPEAT);
		glTextureParameteri(bitmap->object, GL_TEXTURE_WRAP_S, GL_REPEAT);

		bitmap->width = width;
		bitmap->height = height;
		bitmap->format = pixelFormat;
		bitmap->depth = 1;
	}


	BITMAP * bmap_to_mipmap(BITMAP * bitmap)
	{
		Bitmap * bmp = promote<Bitmap>(bitmap);
		if(bmp == nullptr) {
			return nullptr;
		}
		glTextureParameteri(bitmap->object, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTextureParameteri(bitmap->object, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateTextureMipmap(bitmap->object);
		return bitmap;
	}

	void bmap_remove(BITMAP * bitmap)
	{
		Bitmap * bmp = promote<Bitmap>(bitmap);
		if(bmp) {
			delete bmp;
		}
	}
}
