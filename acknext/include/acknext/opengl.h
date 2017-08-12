#ifndef _ACKNEXT_OPENGL_H_
#define _ACKNEXT_OPENGL_H_

#include "config.h"
#include "core-graphics.h"
#include "scene.h"

ACKFUN void opengl_setVertexBuffer(BUFFER const * buffer);

ACKFUN void opengl_setIndexBuffer(BUFFER const * buffer);

ACKFUN void opengl_setShader(SHADER const * shader);

ACKFUN void opengl_setMaterial(MATERIAL const * material);

ACKFUN void opengl_setTexture(int slot, BITMAP const * texture);

ACKFUN void opengl_setMesh(MESH const * mesh);

ACKFUN void opengl_setLight(LIGHT const * mesh);

ACKFUN void opengl_setTransform(MATRIX const * matWorld, MATRIX const * matView, MATRIX const * matProj);

ACKFUN void opengl_draw(
	unsigned int primitiveType,
	unsigned int offset,
	unsigned int count);

ACKVAR int opengl_debugMode;

ACKVAR bool opengl_wireFrame;

#endif // _ACKNEXT_OPENGL_H_
