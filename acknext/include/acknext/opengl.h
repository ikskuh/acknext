#ifndef _ACKNEXT_OPENGL_H_
#define _ACKNEXT_OPENGL_H_

#include "config.h"
#include "core-graphics.h"
#include "scene.h"

ACKVAR int opengl_debugMode;

ACKVAR bool opengl_wireFrame;

ACKFUN void opengl_setFrameBuffer(FRAMEBUFFER * fb);

ACKFUN void opengl_setVertexBuffer(BUFFER const * buffer);

ACKFUN void opengl_setIndexBuffer(BUFFER const * buffer);

ACKFUN void opengl_setShader(SHADER const * shader);

ACKFUN void opengl_setMaterial(MATERIAL const * material);

ACKFUN void opengl_setTexture(int slot, BITMAP const * texture);

ACKFUN void opengl_setLights();

ACKFUN void opengl_setTransform(MATRIX const * matWorld, MATRIX const * matView, MATRIX const * matProj);

ACKFUN GLenum opengl_setMesh(MESH const * mesh, int * count);

ACKFUN void opengl_drawMesh(MESH const * mesh);

// Renders a fullscreen quad for postprocessing purposes
ACKFUN void opengl_drawFullscreenQuad();

ACKFUN void opengl_draw(
	unsigned int primitiveType,
	unsigned int offset,
	unsigned int count,
	unsigned int instances);

ACKFUN void opengl_drawDebug(MATRIX * const matView, MATRIX * const matProj);

#endif // _ACKNEXT_OPENGL_H_
