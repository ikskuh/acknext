#include "engine.h"

ACKFUN void vec_set(VECTOR * dst, VECTOR const * src)
{
	memcpy(dst, src, sizeof(VECTOR));
}

ACKFUN void quat_set(QUATERNION * dst, QUATERNION const * src)
{
	memcpy(dst, src, sizeof(QUATERNION));
}

ACKFUN void vec_zero(VECTOR * dst)
{
	memset(dst, 0, sizeof(VECTOR));
}

ACKFUN void quat_id(QUATERNION * dst)
{
	dst->x = dst->y = dst->z = 0.0f;
	dst->w = 1.0f;
}

ACKFUN VECTOR const * vector(var x, var y, var z)
{
	static int index = 0;
	static VECTOR values[32];

	values[index].x = x;
	values[index].y = y;
	values[index].z = z;

    return &values[index++ % 32];
}

ACKFUN QUATERNION const * euler(var pan, var tilt, var roll)
{
	static int index = 0;
	static QUATERNION values[32];


    QUATERNION & q = values[index++ % 32];

	var rot_x = DEG_TO_RAD * tilt;
	var rot_y = DEG_TO_RAD * pan;
	var rot_z = DEG_TO_RAD * roll;

    var t0 = cos(rot_z * 0.5f);
    var t1 = sin(rot_z * 0.5f);
    var t2 = cos(rot_x * 0.5f);
    var t3 = sin(rot_x * 0.5f);
    var t4 = cos(rot_y * 0.5f);
    var t5 = sin(rot_y * 0.5f);

    q.w = t0 * t2 * t4 + t1 * t3 * t5;
    q.x = t0 * t3 * t4 - t1 * t2 * t5;
    q.y = t0 * t2 * t5 + t1 * t3 * t4;
    q.z = t1 * t2 * t4 - t0 * t3 * t5;

    return &q;
}
