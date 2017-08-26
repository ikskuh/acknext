#ifndef _ACKNEXT_ACKTRANSFORMS_H_
#define _ACKNEXT_ACKTRANSFORMS_H_

#include "config.h"
#include "ackmath.h"
#include "view.h"
#include "scene.h"

ACKFUN VECTOR * vec_to_screen(VECTOR * vec, CAMERA * camera, VIEW * view);

ACKFUN VECTOR * vec_for_screen(VECTOR * vec, CAMERA * camera, VIEW * view);

ACKFUN VECTOR * vec_transform(VECTOR * vec, MATRIX const * mat);

ACKFUN VECTOR4 * vec4_transform(VECTOR4 * vec, MATRIX const * mat);

#endif // _ACKNEXT_ACKTRANSFORMS_H_
