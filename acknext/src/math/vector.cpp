#include <engine.hpp>
#include <math.h>
#include "../graphics/scene/ackglm.hpp"

#define DEFAULT(x) if(!x) x=vector(0,0,0)

ACKNEXT_API_BLOCK
{
	VECTOR nullvector = { 0, 0, 0 };

	VECTOR * vector(var x, var y, var z)
	{
		static VECTOR buffer[ACKNEXT_TEMP_VECTORS];
		static int index = 0;
		VECTOR * vec = &buffer[index++ % ACKNEXT_TEMP_VECTORS];
		vec->x = x;
		vec->y = y;
		vec->z = z;
		return vec;
	}

	VECTOR * vec_clone(VECTOR const * src)
	{
		return vector(src->x, src->y, src->z);
	}

	VECTOR * vec_invert(VECTOR * vec)
	{
		vec->x = -vec->x;
		vec->y = -vec->y;
		vec->z = -vec->z;
		return vec;
	}

	VECTOR * vec_set(VECTOR * lhs, VECTOR const * rhs)
	{
		*lhs = *rhs;
		return lhs;
	}

	VECTOR * vec_zero(VECTOR * lhs)
	{
		return vec_fill(lhs, 0.0);
	}

	VECTOR * vec_fill(VECTOR * lhs, var value)
	{
		DEFAULT(lhs);
		lhs->x = value;
		lhs->y = value;
		lhs->z = value;
		return lhs;
	}

	VECTOR * vec_scale(VECTOR * vec, var scale)
	{
		vec->x *= scale;
		vec->y *= scale;
		vec->z *= scale;
		return vec;
	}

	VECTOR * vec_add(VECTOR * lhs, VECTOR const * rhs)
	{
		lhs->x += rhs->x;
		lhs->y += rhs->y;
		lhs->z += rhs->z;
		return lhs;
	}

	VECTOR * vec_sub(VECTOR * lhs, VECTOR const * rhs)
	{
		lhs->x -= rhs->x;
		lhs->y -= rhs->y;
		lhs->z -= rhs->z;
		return lhs;
	}

	VECTOR * vec_mul(VECTOR * lhs, VECTOR const * rhs)
	{
		lhs->x *= rhs->x;
		lhs->y *= rhs->y;
		lhs->z *= rhs->z;
		return lhs;
	}

	VECTOR * vec_diff(VECTOR * diff, VECTOR const * lhs, VECTOR const * rhs)
	{
		DEFAULT(diff);
		*diff = *lhs;
		vec_sub(diff, rhs);
		return diff;
	}

	var vec_length(VECTOR const * vec)
	{
		return sqrt(vec_dot(vec, vec));
	}

	var vec_dist(VECTOR const * lhs, VECTOR const * rhs)
	{
		float dx = lhs->x - rhs->x;
		float dy = lhs->y - rhs->y;
		float dz = lhs->z - rhs->z;
		return sqrt(dx*dx + dy*dy + dz*dz);
	}

	var vec_dot(VECTOR const * lhs, VECTOR const * rhs)
	{
		return lhs->x * rhs->x
			+ lhs->y * rhs->y
			+ lhs->z * rhs->z;
	}

	VECTOR * vec_cross(VECTOR * cross, VECTOR const * lhs, VECTOR const * rhs)
	{
		DEFAULT(cross);
		cross->x = lhs->y * rhs->z - lhs->z * rhs->y;
		cross->y = lhs->z * rhs->x - lhs->x * rhs->z;
		cross->z = lhs->x * rhs->y - lhs->y * rhs->x;
		return cross;
	}

	VECTOR * vec_reflect(VECTOR * dir, VECTOR const * normal)
	{
		// vRay = vRay + 2*dot(vNormal,-vRay)*vNormal
		vec_add(
			dir,
			vec_scale(
				vec_clone(normal),
				var(2) * vec_dot(
					normal,
					vec_invert(vec_clone(dir))
					)
				)
			);
		return dir;
	}

	VECTOR * vec_lerp(VECTOR * vec, VECTOR const * lhs, VECTOR const * rhs, var factor)
	{
		DEFAULT(vec);
		vec->x = lerp(lhs->x, rhs->x, factor);
		vec->y = lerp(lhs->y, rhs->y, factor);
		vec->z = lerp(lhs->z, rhs->z, factor);
		return vec;
	}

	VECTOR * vec_normalize(VECTOR * vec, var length)
	{
		var len = vec_length(vec);
		if(len == 0) { return vec; }
		var scale = length / len;
		return vec_scale(vec, scale);
	}

	VECTOR * vec_rotate(VECTOR * vec, QUATERNION const * rot)
	{
		glm_to_ack(vec, glm::rotate(ack_to_glm(*rot), ack_to_glm(*vec)));
		return vec;
	}
}
