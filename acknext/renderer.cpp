#include "engine.h"

#include "ackglm.hpp"
#include "level-detail.h"
#include "buffer-detail.h"
#include "bitmap-detail.h"
#include "shader-detail.h"
#include "entity-detail.h"

WIDGET * render_root = nullptr;

static GLuint vao;
static SHADER * defaultShader;
static BITMAP * defaultWhiteTexture;

extern char const * srcVertexShader;
extern char const * srcFragmentShader;

void (APIENTRY render_log)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);

ACKFUN void engine_swap()
{
	SDL_GL_SwapWindow(engine.window);
}

void initialize_renderer()
{
	if(gl3wInit() < 0) {
		engine_log("Failed to initialize OpenGL!");
		abort();
	}

    engine_log("OpenGL Version: %s", glGetString(GL_VERSION));

	glDebugMessageCallback(&render_log, nullptr);

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

	glEnable(GL_DEPTH_TEST);

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

	defaultShader = shader_create();

	if(shader_addSource(defaultShader, VERTEX_SHADER, srcVertexShader) == false) {
		abort();
	}
	if(shader_addSource(defaultShader, FRAGMENT_SHADER, srcFragmentShader) == false) {
		abort();
	}
	if(shader_link(defaultShader) == false) {
		abort();
	}

	opengl_setShader(defaultShader);

	uint32_t white = 0xFFFFFFFF;
	defaultWhiteTexture = bmap_create();
	bmap_set(defaultWhiteTexture, 1, 1, RGBA8, &white);
}

static void draw_view(VIEW * view, const SIZE & size)
{
	(void)view;
	if(view->level == nullptr) {
		// view without a level is just an empty screen
		// portion
		return;
	}

	if(view->pipeline != nullptr) {
		engine_log("Pipeline rendering not supported yet.");
		abort();
	}

	glClearDepth(1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);

	MATRIX matView, matProj;
	view_to_matrix(view, matView, matProj);

	LEVEL * level = view->level;
	for(ENTITY * ent : level->_detail->entities)
	{
		ENTITYdetail & detail = *ent->_detail;
		if(detail.model == nullptr) {
			continue;
		}
		MATRIX matWorld;
		glm_to_ack(matWorld,
			glm::mat4_cast(ack_to_glm(ent->rotation)) *
		    glm::translate(glm::mat4(), ack_to_glm(ent->position)));



		for(MESH & mesh : detail.model->meshes)
		{
			opengl_setMesh(&mesh);

			opengl_setTransform(
				matWorld,
				matView,
				matProj);

			opengl_draw(GL_TRIANGLES, 0, mesh.indexBuffer->size / sizeof(INDEX));
		}
	}
}

static void draw_widget(WIDGET *widget, const POINT & offset, const SIZE & containerSize)
{
	if(widget->type >= WIDGET_USER) {
		engine_log("User widgets are not supported yet.");
		abort();
	}
	SIZE size;
	if((widget->flags & AUTOSIZE) && (!widget->width || !widget->height)) {
		size = containerSize;
	} else {
		size = widget->size;
	}

	int gly = screen_size.height - offset.y - widget->position.y - size.height;

	// Setup render viewport
	glScissor(
		offset.x + widget->position.x,
		gly,
		size.width,
		size.height);
	glViewport(
		offset.x + widget->position.x,
		gly,
		size.width,
		size.height);

	switch(widget->type) {
		case WIDGET_VIEW:
			draw_view(reinterpret_cast<VIEW*>(widget), size);
			break;
		default:
			engine_log("Widget type %d is not supported yet.", widget->type);
			abort();
	}
}

void render_frame()
{
	glViewport(0, 0, screen_size.width, screen_size.height);
    glClearColor(
        screen_color.red,
        screen_color.green,
        screen_color.blue,
        screen_color.alpha);
    glClear(GL_COLOR_BUFFER_BIT);

	if(render_root != nullptr) {
		glEnable(GL_SCISSOR_TEST);
		draw_widget(render_root, POINT(), screen_size);
		glDisable(GL_SCISSOR_TEST);
	}

    engine_swap();
}

static BUFFER * currentVertexBuffer;
static BUFFER * currentIndexBuffer;
static SHADER * currentShader;

ACKFUN void opengl_setVertexBuffer(BUFFER * buffer)
{
	GLuint id;
	if(buffer != nullptr) {
		if(buffer->type != GL_ARRAY_BUFFER) {
			engine_seterror(INVALID_ARGUMENT, "Buffer is not a vertex buffer.");
			return;
		}
		if((buffer->size % sizeof(VERTEX)) != 0) {
			engine_seterror(INVALID_ARGUMENT, "Buffer size is not divisible by vertex size.");
			return;
		}
		id = buffer->id;
	}

	glVertexArrayVertexBuffer(
		vao, 10,
	    id, 0, sizeof(VERTEX));

	currentVertexBuffer = buffer;
}

ACKFUN void opengl_setIndexBuffer(BUFFER * buffer)
{
	GLuint id;
	if(buffer != nullptr) {
		if(buffer->type != GL_ELEMENT_ARRAY_BUFFER) {
			engine_seterror(INVALID_ARGUMENT, "Buffer is not an index buffer.");
			return;
		}
		if((buffer->size % sizeof(INDEX)) != 0) {
			engine_seterror(INVALID_ARGUMENT, "Buffer size is not divisible by index size.");
			return;
		}
		id = buffer->id;
	}

	glVertexArrayElementBuffer(vao, id);

	currentIndexBuffer = buffer;
}

ACKFUN void opengl_setTransform(MATRIX const matWorld, MATRIX const matView, MATRIX const matProj)
{
	int pgm = shader_getObject(currentShader);
	int cnt = shader_getUniformCount(currentShader);
	for(int i = 0; i < cnt; i++)
	{
		UNIFORM const * uni = shader_getUniform(currentShader, i);
		MATRIX mat;
		switch(uni->variable) {
			case MAT_WORLD:
				mat_copy(mat, matWorld);
				break;
			case MAT_VIEW:
				mat_copy(mat, matView);
				break;
			case MAT_PROJ:
				mat_copy(mat, matProj);
				break;

			case MAT_WORLDVIEW: {
				mat_copy(mat, matWorld);
				mat_mul(mat, mat, matView);
				break;
			}
			case MAT_WORLDVIEWPROJ: {
				mat_copy(mat, matWorld);
				mat_mul(mat, mat, matView);
				mat_mul(mat, mat, matProj);
				break;
			}
			case MAT_VIEWPROJ: {
				MATRIX mat;
				mat_copy(mat, matView);
				mat_mul(mat, mat, matProj);
				break;
			}

			default:
				// Skip this filthy uniform setting operation!
				continue;
		}

		engine_log("Matrix %d:\n%1.3f %1.3f %1.3f %1.3f\n%1.3f %1.3f %1.3f %1.3f\n%1.3f %1.3f %1.3f %1.3f\n%1.3f %1.3f %1.3f %1.3f",
			uni->variable,
			mat[0][0], mat[0][1], mat[0][2], mat[0][3],
			mat[1][0], mat[1][1], mat[1][2], mat[1][3],
			mat[2][0], mat[2][1], mat[2][2], mat[2][3],
			mat[3][0], mat[3][1], mat[3][2], mat[3][3]);

		glProgramUniformMatrix4fv(
			pgm,
			uni->location,
			1,
			GL_FALSE,
			&mat[0][0]);
	}
}

ACKFUN void opengl_draw(
	unsigned int primitiveType,
	unsigned int offset,
	unsigned int count)
{
	if(currentIndexBuffer == nullptr || currentVertexBuffer == nullptr) {
		engine_seterror(INVALID_OPERATION, "Either index, vertex or both buffers are not set.");
		return;
	}
	if((offset + count) > (currentIndexBuffer->size / sizeof(INDEX))) {
		engine_seterror(INVALID_ARGUMENT, "offset and count index the index buffer outside of its range.");
		return;
	}
	glDrawElements(
		primitiveType,
		count,
		GL_UNSIGNED_INT,
		(const void *)(sizeof(INDEX) * offset));
}

#define FALLBACK(a, b) ((a) ? (a) : (b))

ACKFUN void opengl_setShader(SHADER * shader)
{
	currentShader = FALLBACK(shader, defaultShader);
	glUseProgram(shader_getObject(currentShader));
}


ACKFUN void opengl_setTexture(int slot, BITMAP * texture)
{
	texture = FALLBACK(texture, defaultWhiteTexture);
	glBindTextureUnit(slot, texture->_detail->id);
}

ACKFUN void opengl_setMesh(MESH * mesh)
{
	if(mesh == nullptr) {
		engine_seterror(INVALID_ARGUMENT, "mesh must not be NULL!");
	}
	opengl_setIndexBuffer(mesh->indexBuffer);
	opengl_setVertexBuffer(mesh->vertexBuffer);
	opengl_setMaterial(mesh->material);
}

ACKFUN void opengl_setMaterial(MATERIAL * material)
{
	if(material == nullptr) {
		engine_seterror(INVALID_ARGUMENT, "material must not be NULL!");
		return;
	}

	// this fallbacks to the defaultShader if none is set.
	opengl_setShader(material->shader);

	int pgm = shader_getObject(currentShader);

	int cnt = shader_getUniformCount(currentShader);
	for(int i = 0; i < cnt; i++)
	{
		UNIFORM const * uni = shader_getUniform(currentShader, i);
		switch(uni->variable) {
			case VEC_COLOR:
				glProgramUniform3f(
					pgm,
					uni->location,
					material->color.red,
					material->color.green,
					material->color.blue);
				break;
			case VEC_ATTRIBUTES:
				glProgramUniform3f(
					pgm,
					uni->location,
					material->roughness,
					material->metallic,
					material->fresnell);
				break;
			case VEC_EMISSION:
				glProgramUniform3f(
					pgm,
					uni->location,
					material->emission.red,
					material->emission.green,
					material->emission.blue);
				break;
			default: break;
		}
	}

	opengl_setTexture(0, material->colorTexture);
	opengl_setTexture(1, material->attributeTexture);
	opengl_setTexture(2, material->emissionTexture);
}

void (APIENTRY render_log)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam)
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

char const * srcVertexShader = R"GLSL(#version 330
	layout(location = 0) in vec3 vPosition;
	layout(location = 3) in vec3 vColor;
	layout(location = 4) in vec2 vUV0;
	layout(location = 5) in vec2 vUV1;

	uniform vec3 vecColor;
	uniform mat4 matWorld;
	uniform mat4 matView;
	uniform mat4 matProj;

	out vec3 color;
	out vec2 uv0, uv1;

	void main() {
		gl_Position = matProj * matView * matWorld * vec4(vPosition, 1);
		color = vColor * vecColor * vec3(uv0, 1);
		uv0 = vUV0;
		uv1 = vUV1;
	}
)GLSL";

char const * srcFragmentShader = R"GLSL(#version 330
	in vec3 color;
	in vec2 uv0;

	uniform sampler2D texColor;

	out vec4 fragment;

	void main() {
		fragment = vec4(color,1) * texture(texColor, uv0);
	}
)GLSL";
