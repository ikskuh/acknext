#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <acknext.h>

#define ACKNEXT_API_BLOCK extern "C"

#define NOCOPY(Class) Class(Class const &) = delete; Class(Class&&) = delete

#include "core/engineobject.hpp"

#include <GL/gl3w.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

struct engine
{
    SDL_Window * window;
    SDL_GLContext context;
};

extern struct engine engine;

void engine_setsdlerror();

#endif // ENGINE_HPP
