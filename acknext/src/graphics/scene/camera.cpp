#include "camera.hpp"
#include "ackglm.hpp"

Camera::Camera() :
    EngineObject<CAMERA>(),
    userCreated(true)
{
	CAMERA & c = api();
	c.arc = 60;
	c.zNear = 0.1;
	c.zFar = 10000;
	c.aspect = 1.0;
	c.type = PERSPECTIVE;
	vec_zero(&c.position);
	quat_id(&c.rotation);
	mat_id(&c.projection);
}

Camera::~Camera()
{

}

ACKNEXT_API_BLOCK
{
	CAMERA * camera_create()
	{
		return demote(new Camera());
	}

	void camera_remove(CAMERA * camera)
	{
		Camera * cam = promote<Camera>(camera);
		if(cam && cam->userCreated) {
			delete cam;
		}
	}

	void camera_to_matrix(
	    CAMERA const * cam,
	    MATRIX * matView,
	    MATRIX * matProjection,
		VIEW const * reference)
	{
		SIZE size;
		view_to_bounds(reference, nullptr, &size);
		glm_to_ack(matView,
			glm::inverse(glm::mat4_cast(ack_to_glm(cam->rotation))) *
			glm::translate(glm::mat4(), -ack_to_glm(cam->position)));
		glm_to_ack(matProjection, glm::perspectiveFov(
		   var(DEG_TO_RAD * cam->arc),
		   var(size.width * cam->aspect),
		   var(size.height),
		   cam->zNear, cam->zFar));
	}
}
