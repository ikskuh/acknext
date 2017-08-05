#ifndef _ACKNEXT_SCENE_H_
#define _ACKNEXT_SCENE_H_

#include "config.h"
#include <stdint.h>
#include "ackmath.h"
#include "core-graphics.h"

typedef struct
{
	SHADER * shader;
	BITMAP * colorTexture;
	BITMAP * attributeTexture;
	BITMAP * emissionTexture;
	COLOR color;
	COLOR emission;
	var roughness;
	var metallic;
	var fresnell;
} MATERIAL;

typedef struct
{
	VECTOR position;
	VECTOR normal;
	VECTOR tangent;
	COLOR color;
	UVCOORD texcoord0;
	UVCOORD texcoord1;
	VECTOR4 weights;
} VERTEX;

typedef struct
{
	BUFFER   * vertexBuffer;
	BUFFER   * indexBuffer;
	MATERIAL * material;
} MESH;

typedef struct
{
	// TODO: Implement something useful here
} MODEL;

/**
 * @ingroup rendering
 * @brief An index of a @ref MESH.
 */
typedef uint32_t INDEX;

// model api:
ACKFUN MODEL * model_load(char const * fileName);

ACKFUN MODEL * model_get(char const * fileName); // uses caching

ACKFUN void model_remove(MODEL * model); // only created or loaded ones

#endif // _ACKNEXT_SCENE_H_
