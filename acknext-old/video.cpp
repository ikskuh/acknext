#include "engine.h"

ACKFUN void video_set(int width, int height, bool fullscreen)
{
    SDL_SetWindowSize(engine.window, width, height);
    SDL_SetWindowFullscreen(engine.window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
}

ACKFUN void video_settitle(char const * title)
{
    SDL_SetWindowTitle(engine.window, title);
}
