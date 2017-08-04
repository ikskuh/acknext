#ifndef _ACKNEXT_MATH_H_
#define _ACKNEXT_MATH_H_

#include "config.h"

#include <stdint.h>

typedef float var;

typedef uint64_t BITFIELD;

typedef struct
{
	var x, y;
} VECTOR2;

typedef struct
{
	var x, y, z;
} VECTOR;

typedef struct
{
	var x, y, z, w;
} VECTOR4;

typedef struct
{
	var w, x, y, z;
} QUATERNION;

typedef struct
{
	var fields[4][4];
} MATRIX;

typedef struct
{
	int x, y;
} POINT;

typedef struct
{
	int width, height;
} SIZE;

typedef struct
{
	var red, green, blue, alpha;
} COLOR;

// var Api:
ACKFUN var lerp(var lhs, var rhs, var factor);

ACKFUN var clamp(var value, var min, var max);

ACKFUN var cycle(var value, var min, var max);

// VECTOR Api:

ACKFUN VECTOR * vector(var x, var y, var z);

ACKFUN VECTOR * vec_clone(VECTOR const * src);

ACKFUN VECTOR * vec_invert(VECTOR * vec);

ACKFUN VECTOR * vec_set(VECTOR * lhs, VECTOR const * rhs);

ACKFUN VECTOR * vec_zero(VECTOR * lhs);

ACKFUN VECTOR * vec_fill(VECTOR * lhs, var value);

ACKFUN VECTOR * vec_add(VECTOR * lhs, VECTOR const * rhs);

ACKFUN VECTOR * vec_mul(VECTOR * lhs, VECTOR const * rhs);

ACKFUN VECTOR * vec_sub(VECTOR * lhs, VECTOR const * rhs);

ACKFUN VECTOR * vec_diff(VECTOR * diff, VECTOR const * lhs, VECTOR const * rhs);

ACKFUN var vec_dist(VECTOR const * lhs, VECTOR const * rhs);

ACKFUN var vec_dot(VECTOR const * lhs, VECTOR const * rhs);

ACKFUN var vec_length(VECTOR const * vec);

ACKFUN VECTOR * vec_cross(VECTOR * cross, VECTOR const * lhs, VECTOR const * rhs);

ACKFUN VECTOR * vec_reflect(VECTOR * dir, VECTOR const * normal);

ACKFUN VECTOR * vec_scale(VECTOR * vec, var scale);

ACKFUN VECTOR * vec_lerp(VECTOR * vec, VECTOR const * lhs, VECTOR const * rhs, var factor);

ACKFUN VECTOR * vec_normalize(VECTOR * vec, var length);

// vec_rotate (VECTOR* vDir, ANGLE* vAng)
// vec_rotateback (VECTOR* vDir, ANGLE* vAng)
// vec_rotateaxis (VECTOR* vDir,VECTOR* vAxis,var angle)
// vec_accelerate( VECTOR* distance,VECTOR* speed,VECTOR* accel,var friction): VECTOR*
// vec_for_angle(VECTOR* vec,ANGLE* ang)
// vec_to_angle (ANGLE* ang, VECTOR* dir);

#endif // _ACKNEXT_MATH_H_
