#include "engine.h"
#include "ackglm.hpp"

ACKFUN VIEW * view_create(WIDGET * parent)
{
    VIEW * view = new VIEW;
    WIDGET & widget = view->widget;

    widget.type = WIDGET_VIEW;
    widget.position = point(0, 0);
    widget.size = size(0, 0);
    widget.parent = parent;
    widget.flags = AUTOSIZE;

    view->position = vector(0, 0, 0);
    view->rotation = euler(0, 0, 0);
    view->pipeline = nullptr;
    view->level = nullptr;
    view->flags = NONE;

	view->arc = 60.0f;
	view->aspect = 1.0f;
	view->zNear = 0.5f;
	view->zFar = 10000.0f;

    return view;
}

ACKFUN void view_remove(VIEW * view)
{
    delete view;
}

// coordinate system:
// fwd:   (1, 0, 0)
// right: (0, 1, 0)
// up:    (0, 0, 1)

ACKFUN void view_to_matrix(VIEW const * view, MATRIX matView, MATRIX matProjection)
{
	if(view == NULL || matView == NULL || matProjection == NULL) {
		engine_seterror(INVALID_ARGUMENT, "One of the arguments of view_to_matrix was NULL.");
		return;
	}
	glm::vec3 lookAt = ack_to_glm(camera->position) + ack_to_glm(camera->rotation) * glm::vec3(1, 0, 0);
	engine_log("vtm: (%f %f %f) (%f %f %f)",
		camera->position.x, camera->position.y, camera->position.z,
		lookAt.x, lookAt.y, lookAt.z);
	glm_to_ack(matView, glm::lookAt(
		ack_to_glm(camera->position),
		lookAt,
		glm::vec3(0,0,1)));
	glm_to_ack(matProjection, glm::perspectiveFov(
	   var(DEG_TO_RAD * view->arc),
	   var(screen_size.width * view->aspect),
	   var(screen_size.height),
	   view->zNear, view->zFar));
}
