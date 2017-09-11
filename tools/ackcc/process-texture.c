#include <stdio.h>
#include <stdlib.h>

#include <acknext.h>
#include <acknext/serialization.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

static ACKGUID const guidBitmap =
{{
     0xb4, 0x0f, 0x64, 0x26,
	 0x2d, 0x6c, 0x44, 0xb3,
	 0xbd, 0x3d, 0x62, 0x9f,
	 0xaa, 0x5a, 0xef, 0xe7
}};

static const uint32_t ACKNEXT_MAGIC = 0xCC41E367;

int process_texture(char const * infile, char const * outfile)
{
	if(IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF | IMG_INIT_WEBP) < 0) {
		fprintf(stderr, "%s", IMG_GetError());
		return EXIT_FAILURE;
	}

	SDL_Surface * surface = IMG_Load(infile);
	if(surface == NULL) {
		fprintf(stderr, "%s", IMG_GetError());
		return EXIT_FAILURE;
	}
	SDL_Surface * converted = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ARGB8888, 0);
	if(converted == NULL) {
		fprintf(stderr, "%s", SDL_GetError());
		SDL_FreeSurface(surface);
		return EXIT_FAILURE;
	}
	surface = converted;

	SDL_LockSurface(surface);

	void * pixels = malloc(4 * surface->w * surface->h);
	memcpy(pixels, surface->pixels, 4 * surface->w * surface->h);

	int width = surface->w;
	int height = surface->h;
	int depth = 1;

	GLenum textureFormat = GL_RGBA8;
	GLenum pixelFormat = GL_BGRA;
	GLenum pixelType = GL_UNSIGNED_BYTE;

	SDL_UnlockSurface(surface);

	SDL_FreeSurface(surface);

	GLsizei bpp = 4; // round up
	GLsizei bufsiz = bpp * width * height * depth;

	ACKFILE * file = file_open_write(outfile);
	if(file == NULL) {
		fprintf(stderr, "Failed to open target file	(%s)", outfile);
		return EXIT_FAILURE;
	}

	file_write(file, "ACKNEXT", 8);
	file_write(file, &ACKNEXT_MAGIC, 4);
	file_write_uint32(file, TYPE_BITMAP);
	file_write_guid(file, guidBitmap);

	file_write_uint32(file, GL_TEXTURE_2D);
	file_write_uint32(file, textureFormat);

	file_write_uint32(file, width);
	file_write_uint32(file, height);
	file_write_uint32(file, depth);

	file_write_uint32(file, pixelFormat);
	file_write_uint32(file, pixelType);
	file_write_uint32(file, bufsiz);
	file_write(file, pixels, bufsiz);

	free(pixels);

	return EXIT_SUCCESS;
}
