#include <engine.hpp>

QUATERNION * quat_temp(QUATERNION * q)
{
	if(q) { return q; }
	static QUATERNION buffer[ACKNEXT_TEMP_QUATERNIONS];
	static int index = 0;
	return &buffer[index++ % ACKNEXT_TEMP_QUATERNIONS];
}

ACKNEXT_API_BLOCK
{
	QUATERNION * euler(var pan, var tilt, var roll)
	{
		return quat_mult(quat_mult(
			quat_axis_angle(nullptr, vector(0, 1, 0), pan),
			quat_axis_angle(nullptr, vector(1, 0, 0), tilt)),
			quat_axis_angle(nullptr, vector(0, 0, 1), roll));
	}

	QUATERNION * quat_id(QUATERNION * quat)
	{
		quat = quat_temp(quat);
		quat->x = 0;
		quat->y = 0;
		quat->z = 0;
		quat->w = 1;
		return quat;
	}

	QUATERNION * quat_axis_angle(QUATERNION * quat, VECTOR const * axis, var angle)
	{
		axis = vec_normalize(vec_clone(axis), 1.0f);
		angle *= DEG_TO_RAD;

		quat = quat_temp(quat);
		quat->x = axis->x * sin(angle / 2);
		quat->y = axis->y * sin(angle / 2);
		quat->z = axis->z * sin(angle / 2);
		quat->w = cos(angle / 2);
		return quat;
	}

	QUATERNION * quat_mult(QUATERNION * lhs, QUATERNION const * rhs)
	{
		VECTOR * l = vector(lhs->x, lhs->y, lhs->z);
		VECTOR * r = vector(rhs->x, rhs->y, rhs->z);

		VECTOR vec;
		vec = *vec_add(vec_add(
			vec_scale(l, rhs->w),
			vec_scale(r, lhs->w)),
			vec_cross(NULL, l, r));
		lhs->w = lhs->w * rhs->w - vec_dot(l, r);
		lhs->x = vec.x;
		lhs->y = vec.y;
		lhs->z = vec.z;

		return lhs;
	}
}
