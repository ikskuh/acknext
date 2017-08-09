#include "graphics/core.hpp"
#include "view.hpp"
#include "engine.hpp"
#include <algorithm>

#include "../opengl/shader.hpp"
#include "../opengl/buffer.hpp"
#include "../opengl/bitmap.hpp"
#include "../scene/camera.hpp"

#include "../debug/debugdrawer.hpp"

GLuint vao;
Shader * defaultShader;
Bitmap * defaultWhiteTexture;

// graphics-resource.cpp
extern char const * srcVertexShader;
extern char const * srcFragmentShader;

ACKNEXT_API_BLOCK
{
	SIZE screen_size;

	var screen_gamma = 2.2;

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

	glCreateVertexArrays(1, &vao);
	glEnableVertexArrayAttrib(vao, 0);
	glEnableVertexArrayAttrib(vao, 1);
	glEnableVertexArrayAttrib(vao, 2);
	glEnableVertexArrayAttrib(vao, 3);
	glEnableVertexArrayAttrib(vao, 4);
	glEnableVertexArrayAttrib(vao, 5);
	glEnableVertexArrayAttrib(vao, 6);
	glVertexArrayAttribFormat(vao,
		0, // position
		3,
		GL_FLOAT,
		GL_FALSE,
		offsetof(VERTEX, position));
	glVertexArrayAttribFormat(vao,
		1, // normal
		3,
		GL_FLOAT,
		GL_FALSE,
		offsetof(VERTEX, normal));
	glVertexArrayAttribFormat(vao,
		2, // tangent
		3,
		GL_FLOAT,
		GL_FALSE,
		offsetof(VERTEX, tangent));
	glVertexArrayAttribFormat(vao,
		3, // color
		4,
		GL_FLOAT,
		GL_FALSE,
		offsetof(VERTEX, color));
	glVertexArrayAttribFormat(vao,
		4, // texcoord0
		2,
		GL_FLOAT,
		GL_FALSE,
		offsetof(VERTEX, texcoord0));
	glVertexArrayAttribFormat(vao,
		5, // texcoord1
		2,
		GL_FLOAT,
		GL_FALSE,
		offsetof(VERTEX, texcoord1));
	glVertexArrayAttribFormat(vao,
		6, // weights
		4,
		GL_FLOAT,
		GL_FALSE,
		offsetof(VERTEX, weights));

	glVertexArrayAttribBinding(vao, 0, 10);
	glVertexArrayAttribBinding(vao, 1, 10);
	glVertexArrayAttribBinding(vao, 2, 10);
	glVertexArrayAttribBinding(vao, 3, 10);
	glVertexArrayAttribBinding(vao, 4, 10);
	glVertexArrayAttribBinding(vao, 5, 10);
	glVertexArrayAttribBinding(vao, 6, 10);

	glBindVertexArray(vao);

	// PHYSFS_mount("/home/felix/project/resource/", "/builtin/", 0);

	SHADER * defaultShader = shader_create();

	BLOB * blobVertexShader = blob_load("/builtin/shaders/object.vert");
	BLOB * blobFragmentShader = blob_load("/builtin/shaders/object.frag");

	if(shader_addSource(defaultShader, VERTEXSHADER, (char*)blobVertexShader->data) == false) {
		abort();
	}
	if(shader_addSource(defaultShader, FRAGMENTSHADER, (char*)blobFragmentShader->data) == false) {
		abort();
	}
	if(shader_link(defaultShader) == false) {
		abort();
	}

	blob_remove(blobVertexShader);
	blob_remove(blobFragmentShader);

	opengl_setShader(defaultShader);

	::defaultShader = promote<Shader>(defaultShader);

	uint32_t white = 0xFFFFFFFF;
	defaultWhiteTexture = promote<Bitmap>(bmap_create(TEX_2D));
	bmap_set(demote(defaultWhiteTexture), 1, 1, FORMAT_RGBA8, &white);

	camera = camera_create();
	promote<Camera>(::camera)->userCreated = false;

	DebugDrawer::initialize();
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

	DebugDrawer::reset();
}

void render_shutdown()
{
	DebugDrawer::shutdown();
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
