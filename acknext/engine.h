#pragma once

#include <acknext.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

void engine_seterror(ERROR code, char const * message, ...);

void engine_setsdlerror();

// Renderer API
void render_frame();
void initialize_renderer();

// Scheduler API
void scheduler_update();
void scheduler_initialize();
void scheduler_shutdown();

struct engine
{
    SDL_Window * window;
    SDL_GLContext context;
};

extern struct engine engine;
