#include <stdio.h>
#include <stdlib.h>

#include <acknext.h>
#include <acknext/serialization.h>
#include <acknext/acff.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

int process_texture(char const * infile, ACKFILE * outfile)
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

	file_write_header(outfile, TYPE_BITMAP, acff_guidBitmap);

	file_write_uint32(outfile, GL_TEXTURE_2D);
	file_write_uint32(outfile, textureFormat);

	file_write_uint32(outfile, width);
	file_write_uint32(outfile, height);
	file_write_uint32(outfile, depth);

	file_write_uint32(outfile, pixelFormat);
	file_write_uint32(outfile, pixelType);
	file_write_uint32(outfile, bufsiz);
	file_write(outfile, pixels, bufsiz);

	free(pixels);

	return EXIT_SUCCESS;
}
