#ifndef _ACKNEXT_CORE_GRAPHICS_H_
#define _ACKNEXT_CORE_GRAPHICS_H_

#include "config.h"
#include "ackmath.h"
#include "ackenum.h"
#include "ackdef.h"
#include "filesys.h"

#include <GL/gl3w.h>

typedef struct
{
	GLuint ACKCONST object;
	GLenum ACKCONST target;
	GLenum ACKCONST format;
	int ACKCONST width;
	int ACKCONST height;
	int ACKCONST depth; // 3D textures ;)
	void * ACKCONST pixels; // when bitmap is loaded, contains raw data
} BITMAP;

typedef struct
{
	GLuint ACKCONST object;
	size_t ACKCONST size;
	GLenum ACKCONST type;
} BUFFER;

typedef struct
{
	GLuint ACKCONST object;
	int ACKCONST textureSlotCount; // number of texture slot this object uses
	SHADERFLAGS ACKCONST flags;
} SHADER;

typedef struct
{
	int ACKCONST block;    // block index, if -1, the uniform is free
	int ACKCONST index;    // index in terms of active uniform
	int ACKCONST location; // location in terms of glUniform*()
	char ACKCONST name[128];
	GLenum ACKCONST type;
	int ACKCONST size;
	SHADERVAR ACKCONST var;
	int ACKCONST textureSlot; // the index of the texture slot this uniform has assigned
} UNIFORM;

// BITMAP api:

ACKFUN BITMAP * bmap_create(GLenum type, GLenum format);

ACKFUN BITMAP * bmap_createblack(int width, int height, GLenum format);

ACKFUN BITMAP * bmap_createpixel(COLOR color);

ACKFUN BITMAP * bmap_load(char const * fileName);

ACKFUN BITMAP * bmap_read(ACKFILE * file);

ACKFUN void bmap_write(ACKFILE * file, BITMAP * bitmap);

ACKFUN void bmap_set(
	BITMAP * bitmap,
	int width, int height,
	GLenum pixelFormat,
	GLenum channelFormat,
	void const * data);

ACKFUN void bmap_remove(BITMAP * bitmap);

ACKFUN BITMAP * bmap_to_mipmap(BITMAP * bitmap);

// BUFFER api:
#define VERTEXBUFFER  GL_ARRAY_BUFFER
#define INDEXBUFFER   GL_ELEMENT_ARRAY_BUFFER
#define UNIFORMBUFFER GL_UNIFORM_BUFFER

ACKFUN BUFFER * buffer_create(GLenum type);

ACKFUN void buffer_set(BUFFER * buffer, size_t size, void const * data);

ACKFUN void buffer_update(BUFFER * buffer, size_t offset, size_t size, void const * data);

#define READONLY GL_READ_ONLY
#define WRITEONLY GL_WRITE_ONLY
#define READWRITE GL_READ_WRITE
ACKFUN void * buffer_map(BUFFER * buffer, GLenum mode);

ACKFUN void buffer_unmap(BUFFER * buffer);

ACKFUN void buffer_remove(BUFFER * buffer);

// SHADER api:
#define VERTEXSHADER   GL_VERTEX_SHADER
#define FRAGMENTSHADER GL_FRAGMENT_SHADER
#define GEOMETRYSHADER GL_GEOMETRY_SHADER
#define TESSCTRLSHADER GL_TESS_CONTROL_SHADER
#define TESSEVALSHADER GL_TESS_EVALUATION_SHADER
#define COMPUTESHADER  GL_COMPUTE_SHADER

ACKFUN SHADER * shader_create();

ACKFUN bool shader_addSource(SHADER * shader, GLenum type, const char * source);

ACKFUN bool shader_addFileSource(SHADER * shader, GLenum type, const char * fileName);

ACKFUN bool shader_link(SHADER * shader);

ACKFUN UNIFORM const * shader_getUniform(SHADER const * shader, int index);

ACKFUN int shader_getUniformCount(SHADER const * shader);

ACKFUN UNIFORM const * shader_getUniformByName(SHADER const * shader, char const * name);

ACKFUN void shader_setUniforms(SHADER * shader, void const * source, bool override);

ACKFUN void shader_logInfo(SHADER const * shader);

ACKFUN void shader_remove(SHADER * shader);

#endif // _ACKNEXT_CORE_GRAPHICS_H_
