#include <acknext.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <chrono>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "engine.h"

struct
{
    ERROR code;
    char message[1024];
} static lasterror = {
    SUCCESS, "Success.",
};

using std::chrono::steady_clock;
using std::chrono::high_resolution_clock;

// This clock point is used for logging
static steady_clock::time_point startupTime;

// This clock point is used for frame time calculations
static high_resolution_clock::time_point lastFrameTime;

static FILE *logfile = nullptr;

struct engine engine;

#define SDL_CHECKED(x, y) if((x) < 0) { engine_setsdlerror(); return y; }

ACKFUN bool engine_open(int argc, char ** argv)
{
    startupTime = std::chrono::steady_clock::now();


    engine_log("Begin initalizing engine.");

    if(compiler_init() == false) {
        return false;
    }

    for(int i = 1; i < argc; i++)
    {
        if(strcmp(argv[i], "-diag") == 0) {
            if(logfile != nullptr) {
                continue;
            }
            logfile = fopen("acklog.txt", "w");
            if(logfile == nullptr) {
                engine_log("Failed to open acklog.txt!");
            }
        }

        if(argv[i][0] != '-')
        {
            // positional file!
            if(compiler_add(argv[i]) == false) {
                return false;
            }
        }
    }

    SDL_CHECKED(SDL_Init(SDL_INIT_EVERYTHING), false)

    SDL_CHECKED(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3), false)
    SDL_CHECKED(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3), false)
    SDL_CHECKED(SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG), false)
    SDL_CHECKED(SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1), false)

    engine.window = SDL_CreateWindow(
        "Acknext A1",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1280, 720,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_MOUSE_FOCUS);
    if(engine.window == nullptr)
    {
        engine_setsdlerror();
        return false;
    }

    engine.context = SDL_GL_CreateContext(engine.window);
    if(engine.context == nullptr)
    {
        engine_setsdlerror();
        return false;
    }

    initialize_renderer();

    scheduler_initialize();

    engine_log("Engine ready.");

    lastFrameTime = high_resolution_clock::now();

    if(compiler_start() == false) {
        return false;
    }

    return true;
}

ACKFUN bool engine_frame()
{
    auto nextFrameTime = high_resolution_clock::now();
    // Time Setup
    {
        std::chrono::duration<float> timePoint;
        timePoint = std::chrono::duration_cast<std::chrono::milliseconds>(
                    nextFrameTime - lastFrameTime);
        time_step = timePoint.count();

        timePoint = std::chrono::duration_cast<std::chrono::microseconds>(
                    steady_clock::now() - startupTime);
        total_secs = timePoint.count();
    }

    SDL_GetWindowSize(engine.window, &screen_size.width, &screen_size.height);

    SDL_Event event;

    // Update Frame
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_QUIT:
            {
                // TODO: Initialize engine shutdown here.
                return false;
            }
        }
    }

    scheduler_update();

    // Render Frame
    render_frame();

    lastFrameTime = nextFrameTime;
    total_frames++;
    return true;
}

ACKFUN void engine_close()
{
    engine_log("Shutting down engine...");

    scheduler_shutdown();

    engine_log("Destroy GL context.");
    SDL_GL_DeleteContext(engine.context);

    engine_log("Close window.");
    SDL_DestroyWindow(engine.window);

    engine_log("Engine shutdown complete.");
}

ACKFUN void engine_log(char const * format, ...)
{
    // TODO: Determine current time
    std::chrono::duration<float> timePoint = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startupTime);

    FILE * files[] = { stderr, logfile };

    for(int i = 0; i < 2; i++)
    {
        if(files[i] == nullptr) {
            continue;
        }
        fprintf(files[i], "%10.4f: ", timePoint.count());
        va_list args;
        va_start(args, format);
        vfprintf(files[i], format, args);
        va_end(args);
        fprintf(files[i], "\n");
    }
}

ACKFUN char const * engine_lasterror(ERROR * errcode)
{
    if(errcode != nullptr)
    {
        *errcode = lasterror.code;
    }
    return lasterror.message;
}

#define STRINGIFY(x) #x

void engine_seterror(ERROR code, char const * message, ...)
{
    static char const * errorNames[] = {
        STRINGIFY(SUCCESS),
        STRINGIFY(OUT_OF_MEMORY),
        STRINGIFY(SDL_ERROR),
        STRINGIFY(COMPILATION_FAILED),
    };

    va_list args;
    va_start(args, message);
    vsnprintf(lasterror.message, 1023, message, args);
    va_end(args);

    lasterror.code = code;

    if(lasterror.code != SUCCESS)
    {
        char const * errname = (lasterror.code < (sizeof(errorNames) / sizeof(errorNames[0]))) ? errorNames[lasterror.code] : "UNKNOWN_ERR";
        engine_log("%s: %s", errname, lasterror.message);
    }
}

void engine_setsdlerror()
{
    engine_seterror(SDL_ERROR, SDL_GetError());
}

HANDLE handle_getnew(int type)
{
    static int objects[64];
    if(type < 1 || type >= 64) {
        return 0;
    } else {
        return ((++objects[type]) << 6) | type;
    }
}