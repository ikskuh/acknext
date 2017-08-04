#include "graphics/core.hpp"
#include "view.hpp"
#include "engine.hpp"
#include <algorithm>

ACKNEXT_API_BLOCK
{
	SIZE screen_size;

	COLOR screen_color = { 0, 0, 0.5, 1.0 };
}

static void (APIENTRY render_log)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);

void render_init()
{
	if(gl3wInit() < 0) {
		engine_log("Failed to initialize OpenGL!");
		abort();
	}

    engine_log("OpenGL Version: %s", glGetString(GL_VERSION));

	glDebugMessageCallback(&render_log, nullptr);

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
}

void render_frame()
{
	std::sort(
		View::all.begin(),
		View::all.end(),
		[](View * lhs, View * rhs) { return (lhs->api().layer < rhs->api().layer); });

	glDisable(GL_SCISSOR_TEST);

	glClearColor(screen_color.red, screen_color.green, screen_color.blue, screen_color.alpha);
	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_SCISSOR_TEST);

	for(View * view : View::all)
	{
		view->draw();
	}

	SDL_GL_SwapWindow(engine.window);
}

void render_shutdown()
{

}

static void (APIENTRY render_log)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam)
{
	(void)source;
	(void)type;
	(void)id;
	(void)length;
	(void)userParam;
	if(severity == GL_DEBUG_SEVERITY_HIGH || severity == GL_DEBUG_SEVERITY_MEDIUM) {
		engine_log("[OpenGL] %s", message);
	}
}
