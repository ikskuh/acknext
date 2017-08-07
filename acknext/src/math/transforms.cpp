#include <engine.hpp>

#include "../graphics/scene/ackglm.hpp"

ACKNEXT_API_BLOCK
{
	// vec→viewspace→screenspace
	VECTOR * vec_to_screen(VECTOR * vec, CAMERA * cam, VIEW * view)
	{
		if(vec == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "vec must not be NULL!");
			return nullptr;
		}

		if(cam == nullptr) {
			cam = camera;
		}
		POINT pos = { 0, 0 };
		SIZE size = screen_size;
		if(view != nullptr) {
			view_to_bounds(view, &pos, &size);
		}

		MATRIX matView, matProj, matViewProj;
		camera_to_matrix(cam, &matView, &matProj, view);

		mat_mul(&matViewProj, &matView, &matProj);

		VECTOR4 vec4;
		vec4.x = vec->x;
		vec4.y = vec->y;
		vec4.z = vec->z;
		vec4.w = 1.0;

		vec4_transform(&vec4, &matViewProj);

		vec->x = vec4.x / vec4.w;
		vec->y = vec4.y / vec4.w;
		vec->z = vec4.z; // / vec4.w;

		vec->x = (size.width - 1) * (0.5 + 0.5 * vec->x);
		vec->y = (size.height - 1) * (0.5 - 0.5 * vec->y);
		// vec->z = cam->zFar + (cam->zFar - cam->zNear) * vec->z;

		return vec;
	}

	// vec→viewspace→worldspace
	VECTOR * vec_for_screen(VECTOR * vec, CAMERA * cam, VIEW * view)
	{
		if(vec == nullptr) {
			engine_seterror(ERR_INVALIDARGUMENT, "vec must not be NULL!");
			return nullptr;
		}

		if(cam == nullptr) {
			cam = camera;
		}
		POINT pos = { 0, 0 };
		SIZE size = screen_size;
		if(view != nullptr) {
			view_to_bounds(view, &pos, &size);
		}

		MATRIX matView, matProj, matViewProj;
		camera_to_matrix(cam, &matView, &matProj, view);

		mat_mul(&matViewProj, &matView, &matProj);
		mat_invert(&matViewProj);

		VECTOR4 vec4;
		vec4.x = vec->x * (2.0 / (size.width - 1)) - 1.0;
		vec4.y = vec->y * (2.0 / (size.height - 1)) - 1.0;
		vec4.z = vec->z;
		vec4.w = 1.0;

		engine_log("(%f %f %f %f)", vec4.x, vec4.y, vec4.z, vec4.w);

		vec4_transform(&vec4, &matViewProj);

		engine_log("(%f %f %f %f)", vec4.x, vec4.y, vec4.z, vec4.w);

		vec->x = vec4.x / vec4.w;
		vec->y = vec4.y / vec4.w;
		vec->z = vec4.z / vec4.w;

		return vec;
	}

	VECTOR * vec_transform(VECTOR *vec, const MATRIX *mat)
	{
		VECTOR4 value = { vec->x, vec->y, vec->z, 1.0 };
		vec4_transform(&value, mat);
		vec->x = value.x;
		vec->y = value.y;
		vec->z = value.z;
		return vec;
	}


	VECTOR4 * vec4_transform(VECTOR4 * vec, const MATRIX *mat)
	{
		VECTOR4 result;
		result.x = mat->fields[0][0] * vec->x
			+ mat->fields[1][0] * vec->y
			+ mat->fields[2][0] * vec->z
			+ mat->fields[3][0] * vec->w;
		result.y = mat->fields[0][1] * vec->x
			+ mat->fields[1][1] * vec->y
			+ mat->fields[2][1] * vec->z
			+ mat->fields[3][1] * vec->w;
		result.z = mat->fields[0][2] * vec->x
			+ mat->fields[1][2] * vec->y
			+ mat->fields[2][2] * vec->z
			+ mat->fields[3][2] * vec->w;
		result.w = mat->fields[0][3] * vec->x
			+ mat->fields[1][3] * vec->y
			+ mat->fields[2][3] * vec->z
			+ mat->fields[3][3] * vec->w;
		*vec = result;
		return vec;
	}
}
