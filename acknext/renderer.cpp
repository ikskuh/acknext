#include "engine.h"

void render_frame()
{
    glClearColor(
        screen_color.red / 255.0f,
        screen_color.green / 255.0f,
        screen_color.blue / 255.0f,
        1.0f);
    glClear(GL_COLOR_BUFFER_BIT);



    SDL_GL_SwapWindow(engine.window);
}


void initialize_renderer()
{
    engine_log("OpenGL Version: %s", glGetString(GL_VERSION));
}
