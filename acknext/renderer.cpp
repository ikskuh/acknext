#include "engine.h"

#include "ackglm.hpp"
#include "level-detail.h"
#include "buffer-detail.h"

WIDGET * render_root = nullptr;

static GLuint vao;

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
		GL_UNSIGNED_BYTE,
		GL_TRUE,
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

	LEVEL * level = view->level;
	for(ENTITY * ent : level->_detail->entities)
	{
		glm::mat4 matWorld =
			glm::mat4_cast(ack_to_glm(ent->rotation)) *
		    glm::translate(glm::mat4(), ack_to_glm(ent->position));

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
    glClearColor(
        screen_color.red / 255.0f,
        screen_color.green / 255.0f,
        screen_color.blue / 255.0f,
        1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

	if(render_root != nullptr) {
		glEnable(GL_SCISSOR_TEST);
		draw_widget(render_root, POINT(), screen_size);
		glDisable(GL_SCISSOR_TEST);
	}

	glViewport(0, 0, screen_size.width, screen_size.height);

    engine_swap();
}

static BUFFER * currentVertexBuffer;
static BUFFER * currentIndexBuffer;

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

ACKFUN void opengl_draw(
	unsigned int primitiveType,
	unsigned int offset,
	unsigned int count)
{
	if(currentIndexBuffer == NULL || currentVertexBuffer == NULL) {
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
