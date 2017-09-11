#include <stdio.h>
#include <getopt.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

void stupidcopy(SDL_Surface * dest, SDL_Surface * source, int offset)
{
	uint8_t * bufi = source->pixels;
	uint8_t * bufo = dest->pixels + offset;
	SDL_LockSurface(source);
	for(int y = 0; y < dest->h; y++)
	{
		for(int x = 0; x < dest->w; x++)
		{
			bufo[y * dest->pitch + dest->format->BytesPerPixel * x]
				= bufi[y * source->pitch + source->format->BytesPerPixel * x + 2];
		}
	}
	SDL_UnlockSurface(source);
}

void stupidfill(SDL_Surface * dest, int value, int offset)
{
	uint8_t * bufo = dest->pixels + offset;
	for(int y = 0; y < dest->h; y++)
	{
		for(int x = 0; x < dest->w; x++)
		{
			bufo[y * dest->pitch + dest->format->BytesPerPixel * x] = value;
		}
	}
}

int main(int argc, char ** argv)
{
	IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF | IMG_INIT_WEBP);

	SDL_Surface * sRoughness = NULL;
	SDL_Surface * sMetalness = NULL;
	SDL_Surface * sFresnell = NULL;
	SDL_Surface * sOcclusion = NULL;
	char const * resultFile = NULL;

	float fRoughness = 1.0;
	float fMetalness = 0.0;
	float fFresnell = 1.0;

	char marker = '=';

	int opt;
	while ((opt = getopt(argc, argv, "a:m:f:r:o:")) != -1) {
		switch (opt) {
			case 'a':
				sOcclusion = IMG_Load(optarg);
				break;
			case 'm':
				if(optarg[0] == marker) {
					fMetalness = atof(optarg + 1);
				} else {
					sMetalness = IMG_Load(optarg);
				}
				break;
			case 'f':
				if(optarg[0] == marker) {
					fFresnell = atof(optarg + 1);
				} else {
					sFresnell = IMG_Load(optarg);
				}
				break;
			case 'r':
				if(optarg[0] == marker) {
					fRoughness = atof(optarg + 1);
				} else {
					sRoughness = IMG_Load(optarg);
				}
				break;
			case 'o':
				resultFile = strdup(optarg);
				break;
			default: /* '?' */
				fprintf(stderr, "Usage: %s [-a ambientmap] [-m metalmap] [-f fresnellmap] [-r roughmap] -o output\n",
				   argv[0]);
				exit(EXIT_FAILURE);
		}
	}
	if(!resultFile)
		abort();

	if(!sMetalness && !sRoughness && !sFresnell && !sOcclusion)
		abort();

	int w,h;
	{
		SDL_Surface * surfs[4] = { sMetalness, sRoughness, sFresnell, sOcclusion };
		int i;
		for(i = 0; i < 4; i++) {
			if(surfs[i]) {
				w = surfs[i]->w;
				h = surfs[i]->h;
				break;
			}
		}
		for(++i; i < 4; i++) {
			if(surfs[i]) {
				if(surfs[i]->w != w) abort();
				if(surfs[i]->h != h) abort();
			}
		}
	}

	if(sMetalness) sMetalness = SDL_ConvertSurfaceFormat(sMetalness, SDL_PIXELFORMAT_RGBA8888, 0);
	if(sRoughness) sRoughness = SDL_ConvertSurfaceFormat(sRoughness, SDL_PIXELFORMAT_RGBA8888, 0);
	if(sFresnell)  sFresnell = SDL_ConvertSurfaceFormat(sFresnell, SDL_PIXELFORMAT_RGBA8888, 0);
	if(sOcclusion) sOcclusion = SDL_ConvertSurfaceFormat(sOcclusion, SDL_PIXELFORMAT_RGBA8888, 0);

	SDL_Surface * target = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGBA8888);

	SDL_LockSurface(target);

	stupidfill(target, 255 * fRoughness, 3); // → r
	stupidfill(target, 255 * fMetalness, 2); // → g
	stupidfill(target, 255 * fFresnell,  1); // → b
	stupidfill(target, 255,              0); // → a

	if(sRoughness) stupidcopy(target, sRoughness, 3); // → r
	if(sMetalness) stupidcopy(target, sMetalness, 2); // → g
	if(sFresnell)  stupidcopy(target, sFresnell,  1); // → b
	if(sOcclusion) stupidcopy(target, sOcclusion, 0); // → a

	SDL_UnlockSurface(target);

	IMG_SavePNG(target, resultFile);

	return 0;
}
