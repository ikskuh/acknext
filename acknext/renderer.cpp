#include "engine.h"

#include "ackglm.hpp"
#include "level-detail.h"

WIDGET * render_root = nullptr;

static void draw_widget(WIDGET * widget, const POINT & offset, const SIZE & size);

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

    SDL_GL_SwapWindow(engine.window);
}

void initialize_renderer()
{
    engine_log("OpenGL Version: %s", glGetString(GL_VERSION));
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
