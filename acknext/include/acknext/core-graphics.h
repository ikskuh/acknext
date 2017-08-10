#ifndef _ACKNEXT_CORE_GRAPHICS_H_
#define _ACKNEXT_CORE_GRAPHICS_H_

#include "config.h"
#include "ackmath.h"
#include "ackenum.h"

typedef unsigned int GLDATA;

typedef struct
{
	TEXTURETYPE ACKCONST type;
	int ACKCONST width;
	int ACKCONST height;
	int ACKCONST depth; // 3D textures ;)
	PIXELFORMAT ACKCONST format;
} BITMAP;

typedef struct
{
	size_t ACKCONST size;
	BUFFERTYPE ACKCONST type;
} BUFFER;

typedef struct
{

} SHADER;

typedef struct
{
	int ACKCONST location;
	char ACKCONST name[128];
	GLDATA ACKCONST type;
	int ACKCONST size;
	SHADERVAR ACKCONST var;
} UNIFORM;

// BITMAP api:

ACKFUN BITMAP * bmap_create(TEXTURETYPE type);

ACKFUN BITMAP * bmap_createblack(int width, int height, PIXELFORMAT format);

ACKFUN BITMAP * bmap_load(char const * fileName);

ACKFUN void bmap_set(BITMAP * bitmap, int width, int height, PIXELFORMAT format, void const * data);

ACKFUN void bmap_remove(BITMAP * bitmap);

ACKFUN BITMAP * bmap_to_mipmap(BITMAP * bitmap);

// BUFFER api:
ACKFUN BUFFER * buffer_create(BUFFERTYPE type);

ACKFUN void buffer_set(BUFFER * buffer, size_t size, void * data);

ACKFUN void buffer_update(BUFFER * buffer, size_t offset, size_t size, void * data);

ACKFUN GLDATA buffer_getObject(BUFFER * buffer);

ACKFUN void buffer_remove(BUFFER * buffer);

// SHADER api:
ACKFUN SHADER * shader_create();

ACKFUN bool shader_addSource(SHADER * shader, SHADERTYPE type, const char * source);

ACKFUN bool shader_link(SHADER * shader);

ACKFUN UNIFORM const * shader_getUniform(SHADER * shader, int index);

ACKFUN int shader_getUniformCount(SHADER * shader);

ACKFUN GLDATA shader_getObject(SHADER * shader);

ACKFUN void shader_remove(SHADER * shader);

#endif // _ACKNEXT_CORE_GRAPHICS_H_
