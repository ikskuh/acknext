#ifndef _ACKNEXT_SCENE_H_
#define _ACKNEXT_SCENE_H_

#include "config.h"
#include <stdint.h>
#include "ackmath.h"
#include "ackenum.h"
#include "core-graphics.h"
#include "view.h"

typedef struct
{
	SHADER * shader;
	BITMAP * colorTexture;
	BITMAP * attributeTexture; // r=roughness, g=metallic, b=fresnell, a=ambient occlusion
	BITMAP * emissionTexture;
	BITMAP * normalTexture; // r,g,b∊[-1;1]
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

typedef struct
{
	VECTOR position;
	QUATERNION rotation;
	var zNear, zFar;
	var arc;
	var aspect;
	MATRIX projection;
	CAMERATYPE type;
} CAMERA;

typedef enum LIGHTTYPE
{
	AMBIENTLIGHT = 0,
	POINTLIGHT = 1,
	SUNLIGHT = 2, // directional
	SPOTLIGHT = 3,
} LIGHTTYPE;

typedef struct
{
	LIGHTTYPE ACKCONST type;
	VECTOR position;
	VECTOR direction;
	COLOR color;
	var intensity; // kinda range, but not completly
	var arc;
} LIGHT;

/**
 * @ingroup rendering
 * @brief An index of a @ref MESH.
 */
typedef uint32_t INDEX;

// model api:
ACKFUN MODEL * model_load(char const * fileName);

ACKFUN MODEL * model_get(char const * fileName); // uses caching

ACKFUN void model_remove(MODEL * model); // only created or loaded ones

// render api:
ACKVAR CAMERA * ACKCONST camera;

ACKVAR COLOR sky_color;

ACKFUN CAMERA * camera_create();

ACKFUN void camera_remove(CAMERA * camera);

ACKFUN void camera_to_matrix(
    CAMERA const * view,
    MATRIX * matView,
    MATRIX * matProjection,
	VIEW const * reference);

ACKFUN void render_scene_with_camera(CAMERA * camera);

// material api:

ACKFUN MATERIAL * mtl_create();
ACKFUN void mtl_remove(MATERIAL * mtl);

// light api:
ACKFUN LIGHT * light_create(LIGHTTYPE type);

ACKFUN void light_remove(LIGHT * light);

ACKFUN LIGHT * light_next(LIGHT const * light);

#endif // _ACKNEXT_SCENE_H_
