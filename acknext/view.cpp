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

    vec_zero(&view->position);
    quat_id(&view->rotation);
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
// fwd:   (0, 0, 1)
// right: (1, 0, 0)
// up:    (0, 1, 0)

ACKFUN void view_to_matrix(VIEW const * view, MATRIX * matView, MATRIX * matProjection)
{
	if(view == NULL || matView == NULL || matProjection == NULL) {
		engine_seterror(INVALID_ARGUMENT, "One of the arguments of view_to_matrix was NULL.");
		return;
	}

	glm::mat4 ackToGL(
		 0,  0, -1,  0, // x → -z
		 1,  0,  0,  0, // y → +x
		 0,  1,  0,  0, // z → +y
		 0,  0,  0,  1);

	glm_to_ack(matView,
		glm::mat4_cast(ack_to_glm(view->rotation)) *
		glm::translate(glm::mat4(), -ack_to_glm(view->position)));

	// glm_to_ack(matView, glm::lookAt(glm::vec3(0,0,0), glm::vec3(0,0,-1), glm::vec3(0, 1, 0)));

	MATRIX mat = *matView;

	engine_log("q=(%f %f %f %f)", view->rotation.x, view->rotation.y, view->rotation.z, view->rotation.w);
	engine_log("Matrix %d:\n%1.3f %1.3f %1.3f %1.3f\n%1.3f %1.3f %1.3f %1.3f\n%1.3f %1.3f %1.3f %1.3f\n%1.3f %1.3f %1.3f %1.3f",
		99,
		mat.v[0][0], mat.v[0][1], mat.v[0][2], mat.v[0][3],
		mat.v[1][0], mat.v[1][1], mat.v[1][2], mat.v[1][3],
		mat.v[2][0], mat.v[2][1], mat.v[2][2], mat.v[2][3],
		mat.v[3][0], mat.v[3][1], mat.v[3][2], mat.v[3][3]);

	/*
	glm_to_ack(matView, glm::lookAt(
		glm::vec3(0, 0, 0),
		glm::vec3(0, 0, 10),
		glm::vec3(0, 1, 0)));
	*/
	glm_to_ack(matProjection, glm::perspectiveFov(
	   var(DEG_TO_RAD * view->arc),
	   var(screen_size.width * view->aspect),
	   var(screen_size.height),
	   view->zNear, view->zFar));
}
