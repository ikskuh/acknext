#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <acknext.h>

#define ACKNEXT_API_BLOCK extern "C"

#define NOCOPY(Class) Class(Class const &) = delete; Class(Class&&) = delete

#include "core/engineobject.hpp"

#include <GL/gl3w.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>

struct engine
{
    SDL_Window * window;
    SDL_GLContext context;
};

extern struct engine engine;

void engine_setsdlerror();

void _print_stacktrace();

#define ARG_NOTNULL(arg,val) if(arg == nullptr) { \
	engine_seterror(ERR_INVALIDARGUMENT, #arg " must not be NULL!"); \
	return val; \
}

#endif // ENGINE_HPP
