#include "graphics/core.hpp"
#include "view.hpp"
#include <engine.hpp>
#include <algorithm>

#include "../opengl/shader.hpp"
#include "../opengl/buffer.hpp"
#include "../opengl/bitmap.hpp"
#include "../scene/camera.hpp"

#include "../debug/debugdrawer.hpp"

#include "../shareddata.hpp"

GLuint vao;
Shader * defaultShader;
BITMAP * defaultWhiteTexture;
BITMAP * defaultNormalMap;

MESH   * fullscreenQuad;

// graphics-resource.cpp
extern char const * srcVertexShader;
extern char const * srcFragmentShader;

ACKNEXT_API_BLOCK
{
	SIZE screen_size;

	var screen_gamma = 2.2;

	COLOR screen_color = { 0, 0, 0.5, 1.0 };

	VIEW * view_current;
}

static GLuint renderTimeQuery;

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
	glEnableVertexArrayAttrib(vao, 7);

	glEnableVertexArrayAttrib(vao, 8);
	glEnableVertexArrayAttrib(vao, 9);
	glEnableVertexArrayAttrib(vao, 10);
	glEnableVertexArrayAttrib(vao, 11);

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
		6, // ids
		4,
		GL_UNSIGNED_BYTE,
		GL_FALSE,
		offsetof(VERTEX, bones));
	glVertexArrayAttribFormat(vao,
		7, // weights
		4,
		GL_UNSIGNED_BYTE,
		GL_TRUE,
		offsetof(VERTEX, boneWeights));


	glVertexArrayAttribFormat(vao,
		8, // instancing transform
		4,
		GL_FLOAT,
		GL_FALSE,
		0);
	glVertexArrayAttribFormat(vao,
		9, // instancing transform
		4,
		GL_FLOAT,
		GL_FALSE,
		16);
	glVertexArrayAttribFormat(vao,
		10, // instancing transform
		4,
		GL_FLOAT,
		GL_FALSE,
		32);
	glVertexArrayAttribFormat(vao,
		11, // instancing transform
		4,
		GL_FLOAT,
		GL_FALSE,
		48);

	glVertexArrayAttribBinding(vao,  0, 10);
	glVertexArrayAttribBinding(vao,  1, 10);
	glVertexArrayAttribBinding(vao,  2, 10);
	glVertexArrayAttribBinding(vao,  3, 10);
	glVertexArrayAttribBinding(vao,  4, 10);
	glVertexArrayAttribBinding(vao,  5, 10);
	glVertexArrayAttribBinding(vao,  6, 10);
	glVertexArrayAttribBinding(vao,  7, 10);

	glVertexArrayAttribBinding(vao,  8, 12);
	glVertexArrayAttribBinding(vao,  9, 12);
	glVertexArrayAttribBinding(vao, 10, 12);
	glVertexArrayAttribBinding(vao, 11, 12);

	glBindVertexArray(vao);

	defaultWhiteTexture = bmap_createpixel(COLOR_WHITE);
	defaultNormalMap = bmap_createpixel((COLOR){0.5, 0.5, 1.0, 1.0});

	// PHYSFS_mount("/home/felix/project/resource/", "/builtin/", 0);

	SHADER * defaultShader = shader_create();

	if(shader_addFileSource(defaultShader, VERTEXSHADER, "/builtin/shaders/object.vert") == false) {
		abort();
	}
	if(shader_addFileSource(defaultShader, FRAGMENTSHADER, "/builtin/shaders/object.frag") == false) {
		abort();
	}
	if(shader_addFileSource(defaultShader, FRAGMENTSHADER, "/builtin/shaders/lighting.glsl") == false) {
		abort();
	}
	if(shader_addFileSource(defaultShader, FRAGMENTSHADER, "/builtin/shaders/gamma.glsl") == false) {
		abort();
	}
	if(shader_addFileSource(defaultShader, FRAGMENTSHADER, "/builtin/shaders/ackpbr.glsl") == false) {
		abort();
	}
	if(shader_addFileSource(defaultShader, FRAGMENTSHADER, "/builtin/shaders/fog.glsl") == false) {
		abort();
	}
	if(shader_link(defaultShader) == false) {
		abort();
	}
	defaultShader->flags |= USE_INSTANCING;

	opengl_setShader(defaultShader);

	shader_setvar(defaultShader, "texNormalMap",  GL_SAMPLER_2D, defaultNormalMap);
	shader_setvar(defaultShader, "texAlbedo",     GL_SAMPLER_2D, defaultWhiteTexture);
	shader_setvar(defaultShader, "texAttributes", GL_SAMPLER_2D, defaultWhiteTexture);
	shader_setvar(defaultShader, "texEmission",   GL_SAMPLER_2D, defaultWhiteTexture);

	::defaultShader = promote<Shader>(defaultShader);

	camera = camera_create();
	promote<Camera>(::camera)->userCreated = false;

	{
		glCreateQueries(GL_TIME_ELAPSED, 1, &renderTimeQuery);


		int num;
		glGetQueryiv(GL_TIME_ELAPSED, GL_QUERY_COUNTER_BITS, &num);
		engine_log("Query Resolution: %d", num);

	}

	{
		VERTEX vertices[4] =
		{
		    {
		        position:    { 0, 0, 0 },
		        normal:      { 0, 0, 1 },
		        tangent:     { 1, 0, 0 },
		        color:       { 1, 1, 1, 1 },
		        texcoord0:   { 0, 0 },
		        texcoord1:   { 0, 0 },
		        bones:       { 0, 0, 0, 0 },
		        boneWeights: { 255, 0, 0, 0 },
		    },
		    {
		        position:    { 0, 0, 0 },
		        normal:      { 0, 0, 1 },
		        tangent:     { 1, 0, 0 },
		        color:       { 1, 1, 1, 1 },
		        texcoord0:   { 0, 0 },
		        texcoord1:   { 0, 0 },
		        bones:       { 0, 0, 0, 0 },
		        boneWeights: { 255, 0, 0, 0 },
		    },
		    {
		        position:    { 0, 0, 0 },
		        normal:      { 0, 0, 1 },
		        tangent:     { 1, 0, 0 },
		        color:       { 1, 1, 1, 1 },
		        texcoord0:   { 0, 0 },
		        texcoord1:   { 0, 0 },
		        bones:       { 0, 0, 0, 0 },
		        boneWeights: { 255, 0, 0, 0 },
		    },
		    {
		        position:    { 0, 0, 0 },
		        normal:      { 0, 0, 1 },
		        tangent:     { 1, 0, 0 },
		        color:       { 1, 1, 1, 1 },
		        texcoord0:   { 0, 0 },
		        texcoord1:   { 0, 0 },
		        bones:       { 0, 0, 0, 0 },
		        boneWeights: { 255, 0, 0, 0 },
		    }

		};
		BUFFER * vbuf = buffer_create(VERTEXBUFFER);
		buffer_set(vbuf, sizeof(vertices), vertices);

		fullscreenQuad = mesh_create(GL_TRIANGLE_STRIP, vbuf, nullptr);
	}

	DebugDrawer::initialize();
}

void render_frame()
{
	engine_stats.drawcalls = 0;
	engine_stats.gpuTime = 0.0;

	std::sort(
		View::all.begin(),
		View::all.end(),
		[](View * lhs, View * rhs) { return (lhs->api().layer < rhs->api().layer); });

	glBeginQuery(GL_TIME_ELAPSED, renderTimeQuery);

	glDisable(GL_SCISSOR_TEST);

	glClearColor(screen_color.red, screen_color.green, screen_color.blue, screen_color.alpha);
	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_SCISSOR_TEST);

	for(View * view : View::all)
	{
		view_current = demote(view);
		view->draw();
		view_current = nullptr;
	}

	glEndQuery(GL_TIME_ELAPSED);

	SDL_GL_SwapWindow(engine.window);
	glDisable(GL_SCISSOR_TEST);

	DebugDrawer::reset();

	ulong time;
	glGetQueryObjectui64v(renderTimeQuery, GL_QUERY_RESULT, &time);
	engine_stats.gpuTime = time / 1000000.0;
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
	if(severity == GL_DEBUG_SEVERITY_HIGH) {
		engine_log("[OpenGL] %s", message);
		_print_stacktrace();
	}
}
