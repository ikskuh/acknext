#ifndef _ACKNEXT_MATH_H_
#define _ACKNEXT_MATH_H_

#include "config.h"

#include <stdint.h>

typedef float var;

typedef uint64_t BITFIELD;

#define BITFIELD_NONE 0
#define BITFIELD_ALL ((uint64_t)-1)

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
	var x, y, z, w;
} QUATERNION;

typedef struct
{
	var fields[4][4];
} MATRIX;

typedef struct
{
	var u, v;
} UVCOORD;

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

typedef union UBYTE4
{
	uint8_t values[4];
	struct { uint8_t x, y, z, w; };
} UBYTE4;

typedef struct AABB
{
	VECTOR minimum;
	VECTOR maximum;
} AABB;

ACKVAR VECTOR ACKCONST nullvector;

//
ACKFUN bool aabb_valid(AABB const * aabb);

// Makes aabb invalid (minimum > maximum)
ACKFUN void aabb_invalidate(AABB * aabb);

// var Api:
ACKFUN var maxv(var a, var b);
ACKFUN var minv(var a, var b);

ACKFUN var lerp(var lhs, var rhs, var factor);

ACKFUN var clamp(var value, var min, var max);

ACKFUN var cycle(var value, var min, var max);

ACKFUN var oscillate(var value, var min, var max);

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

ACKFUN VECTOR * vec_rotate(VECTOR * vec, QUATERNION const * rot);

// vec_rotate (VECTOR* vDir, ANGLE* vAng)
// vec_rotateback (VECTOR* vDir, ANGLE* vAng)
// vec_rotateaxis (VECTOR* vDir,VECTOR* vAxis,var angle)
// vec_accelerate( VECTOR* distance,VECTOR* speed,VECTOR* accel,var friction): VECTOR*
// vec_for_angle(VECTOR* vec,ANGLE* ang)
// vec_to_angle (ANGLE* ang, VECTOR* dir);

// MATRIX Api:

ACKFUN MATRIX * mat_id(MATRIX * mat);

ACKFUN MATRIX * mat_copy(MATRIX * dst, MATRIX const * src);

ACKFUN MATRIX * mat_mul(MATRIX * dst, MATRIX const * lhs, MATRIX const * rhs);

ACKFUN MATRIX * mat_invert(MATRIX * mat);

ACKFUN MATRIX * mat_scale(MATRIX * mat, VECTOR const * scale);
ACKFUN MATRIX * mat_translate(MATRIX * mat, VECTOR const * offset);
ACKFUN MATRIX * mat_rotate(MATRIX * mat, QUATERNION const * rotation);

// QUATERNION Api:

ACKFUN QUATERNION * euler(var pan, var tilt, var roll);

ACKFUN QUATERNION * quat_id(QUATERNION * quat);

ACKFUN QUATERNION * quat_axis_angle(QUATERNION * quat, VECTOR const * axis, var angle);

ACKFUN QUATERNION * quat_set(QUATERNION * lhs, QUATERNION const * rhs);

ACKFUN QUATERNION * quat_mult(QUATERNION * lhs, QUATERNION const * rhs);

ACKFUN MATRIX * quat_to_mat(MATRIX * dst, QUATERNION const * src);

// COLOR Api:
ACKFUN COLOR * color_rgb(int r, int g, int b);
ACKFUN COLOR * color_rgba(int r, int g, int b, int a);
ACKFUN COLOR * color_hex(int hexcode); // 0xFF00FF
ACKFUN COLOR * color_hexa(int hexcode); // 0xFFFF00FF
ACKFUN COLOR * color_addgamma(COLOR * c);
ACKFUN COLOR * color_removegamma(COLOR * c);
ACKFUN COLOR * color_blend(COLOR * dst, COLOR const * left, COLOR const * right, var f);

#endif // _ACKNEXT_MATH_H_
