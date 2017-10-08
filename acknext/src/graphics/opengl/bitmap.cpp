#include "bitmap.hpp"

#include <assert.h>
#include <SDL2/SDL_image.h>

#include "extensions/extension.hpp"
#include <acknext/serialization.h>

Bitmap::Bitmap(GLenum type, GLenum format)
{
	api().target = type;
	api().format = format;
	glCreateTextures(api().target, 1, &api().object);
	assert(api().object);
}

Bitmap::~Bitmap()
{
	if(api().pixels) {
		free(api().pixels);
	}
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

	BITMAP * bmap_createpixel(COLOR color)
	{
		BITMAP * bmp = bmap_createblack(1, 1, GL_RGBA32F);
		glTextureSubImage2D(
			bmp->object,
			0,
			0, 0, 1, 1,
			GL_RGBA, GL_FLOAT,
			&color);
		return bmp;
	}

	BITMAP * bmap_load(char const * fileName)
	{
		ACKFILE * file = file_open_read(fileName);
		if(file == nullptr) {
			return nullptr;
		}

		char const * ext = strrchr(fileName, '.');
		if(ext != nullptr) {
			ext++;
		}

		if(strcasecmp(ext, "atx")  == 0) {
			BITMAP * bmp = bmap_read(file);
			file_close(file);
			return bmp;
		}

		SDL_RWops * rwops = SDL_RWFromAcknext(file);
		if(rwops == nullptr)
			return nullptr;

		SDL_Surface * surface;
		if(ext)
			surface = IMG_LoadTyped_RW(rwops, 1, ext);
		else
			surface = IMG_Load_RW(rwops, 1);
		if(surface == nullptr) {
			engine_setsdlerror();
			return nullptr;
		}
		SDL_Surface * converted = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ARGB8888, 0);
		if(converted == nullptr) {
			engine_setsdlerror();
			SDL_FreeSurface(surface);
			return nullptr;
		}
		surface = converted;

		BITMAP * bmp = bmap_create(GL_TEXTURE_2D, GL_RGBA8);

		SDL_LockSurface(surface);

		bmp->pixels = malloc(4 * surface->w * surface->h);

		const size_t stride = 4 * surface->w;
		for(int y = (surface->h - 1); y >= 0; y--)
		{
			memcpy(
				reinterpret_cast<uint8_t*>(bmp->pixels) + (stride * (surface->h - y - 1)),
				reinterpret_cast<uint8_t*>(surface->pixels) + (stride * y),
				stride);
		}

		bmap_set(bmp, surface->w, surface->h, GL_BGRA, GL_UNSIGNED_BYTE, bmp->pixels);
		SDL_UnlockSurface(surface);

		SDL_FreeSurface(surface);
		return bmp;
	}

	void bmap_renew(BITMAP * bitmap)
	{
		ARG_NOTNULL(bitmap,);

		if(bitmap->pixels)
			free(bitmap->pixels);

		glDeleteTextures(1, &bitmap->object);
		glCreateTextures(bitmap->target, 1, &bitmap->object);

		bitmap->pixels = nullptr;
		bitmap->width = 0;
		bitmap->height = 0;
		bitmap->depth = 0;
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
		bitmap->depth = 1;
	}

	BITMAP * bmap_to_mipmap(BITMAP * bitmap)
	{
		ARG_NOTNULL(bitmap, nullptr);
		glTextureParameteri(bitmap->object, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTextureParameteri(bitmap->object, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateTextureMipmap(bitmap->object);
		return bitmap;
	}

	BITMAP * bmap_to_linear(BITMAP * bitmap)
	{
		ARG_NOTNULL(bitmap, nullptr);
		glTextureParameteri(bitmap->object, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(bitmap->object, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		return bitmap;
	}

	BITMAP * bmap_to_nearest(BITMAP * bitmap)
	{
		ARG_NOTNULL(bitmap, nullptr);
		glTextureParameteri(bitmap->object, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(bitmap->object, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
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
