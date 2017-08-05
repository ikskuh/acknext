#ifndef _ACKNEXT_OPENGL_H_
#define _ACKNEXT_OPENGL_H_

#include "config.h"
#include "core-graphics.h"
#include "scene.h"

ACKFUN void opengl_setVertexBuffer(BUFFER * buffer);

ACKFUN void opengl_setIndexBuffer(BUFFER * buffer);

ACKFUN void opengl_setShader(SHADER * shader);

ACKFUN void opengl_setMaterial(MATERIAL * material);

ACKFUN void opengl_setTexture(int slot, BITMAP * texture);

ACKFUN void opengl_setMesh(MESH * mesh);

ACKFUN void opengl_setTransform(MATRIX const * matWorld, MATRIX const * matView, MATRIX const * matProj);

ACKFUN void opengl_draw(
	unsigned int primitiveType,
	unsigned int offset,
	unsigned int count);

#endif // _ACKNEXT_OPENGL_H_
