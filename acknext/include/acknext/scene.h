#ifndef _ACKNEXT_SCENE_H_
#define _ACKNEXT_SCENE_H_

#include "config.h"
#include <stdint.h>
#include "ackmath.h"
#include "ackenum.h"
#include "core-graphics.h"
#include "view.h"

#define LOD0		(1<<0)
#define LOD1		(1<<1)
#define LOD2		(1<<2)
#define LOD3		(1<<3)
#define LOD4		(1<<4)
#define LOD5		(1<<5)
#define LOD6		(1<<6)
#define LOD7		(1<<7)
#define LOD8		(1<<8)
#define LOD9		(1<<9)
#define LOD10		(1<<10)
#define LOD11		(1<<11)
#define LOD12		(1<<12)
#define LOD13		(1<<13)
#define LOD14		(1<<14)
#define LOD15		(1<<15)
#define DOUBLESIDED (1<<16)

// Plain type, has no backend
typedef struct
{
	VECTOR position;
	VECTOR normal;
	VECTOR tangent;
	COLOR color;
	UVCOORD texcoord0;
	UVCOORD texcoord1;
	UBYTE4 bones;
	UBYTE4 boneWeights;
} VERTEX;

// Plain type, has no backend
typedef struct
{
	uint8_t parent;
	char name[64];
	// VECTOR offset;
	// QUATERNION rotation;
	MATRIX transform;
	MATRIX bindToBoneTransform;
} BONE;

typedef struct
{
	SHADER * shader;
	BITMAP * albedoTexture;
	BITMAP * attributeTexture; // r=roughness, g=metallic, b=fresnell, a=ambient occlusion
	BITMAP * emissionTexture;
	BITMAP * normalTexture; // r,g,b∊[-1;1]
	COLOR albedo;
	COLOR emission;
	var roughness;
	var metallic;
	var fresnell;
} MATERIAL;

typedef struct
{
	GLenum ACKCONST primitiveType;
	BUFFER * vertexBuffer;
	BUFFER * indexBuffer;
	AABB boundingBox;
	uint32_t lodMask; // lower 16 bits: lod stage, upper 16 bits: render flags!
} MESH;

// Plain type, no backend
typedef struct
{
	var time; // starting time of the frame
	VECTOR position;
	QUATERNION rotation;
	VECTOR scale;
} FRAME;

// managed type
typedef struct
{
	int targetBone; // The bone that will be animated
	int frameCount;
	FRAME * ACKCONST frames;
} CHANNEL;

// managed type
typedef struct
{
	char name[256];
	var duration;
	int channelCount;
	CHANNEL * * ACKCONST channels;
	ANIMFLAGS flags;
} ANIMATION;

// managed type
typedef struct
{
	// counts have a minimum of 1, except for animation count
	int ACKCONST meshCount;
	int ACKCONST animationCount;
	int ACKCONST boneCount;

	// Materials are 1:1 mapped to meshes!
	MESH      * /*ACKCONST*/ * ACKCONST meshes;     // assigned by model_create or model_reshape
	MATERIAL  * /*ACKCONST*/ * ACKCONST materials;  // assigned by model_create or model_reshape
	ANIMATION * /*ACKCONST*/ * ACKCONST animations; // assigned by model_create or model_reshape
	BONE bones[ACKNEXT_MAX_BONES];    // 0 is root/default bone

	AABB ACKCONST boundingBox;
	uint minimumLOD; // (if(lod > minimumLOD) discard; // Usually 16, so always visible
} MODEL;

// managed type
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

// managed type
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

ACKFUN void model_write(ACKFILE * file, MODEL const * model);

ACKFUN MODEL * model_read(ACKFILE * file);

ACKFUN void model_remove(MODEL * model); // only created or loaded ones

ACKFUN MODEL * model_create(int numMeshes, int numBones, int numAnimations);

ACKFUN void model_reshape(MODEL * model, int meshC, int matC, int boneC, int animC);

ACKFUN void model_updateBoundingBox(MODEL * model);

// animation api:

ACKFUN CHANNEL * chan_create(int frames);

ACKFUN void chan_remove(CHANNEL * chan);

ACKFUN ANIMATION * anim_create(char const * name, int channels);

ACKFUN void anim_remove(ANIMATION * anim);

// render api:
ACKVAR var lod_distances[16]; // The distances for each of the 16 LOD stages. Should be strictly monotonically increasing

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

// mesh api:
ACKFUN MESH * mesh_create(GLenum primitiveType, BUFFER * vertexBuffer, BUFFER * indexBuffer);

ACKFUN MESH * mesh_read(ACKFILE * file);

ACKFUN void mesh_write(ACKFILE * file, MESH const * mesh);

ACKFUN void mesh_remove(MESH * mesh);

ACKFUN void mesh_updateBoundingBox(MESH * mesh);

// material api:

ACKFUN MATERIAL * mtl_create();

ACKFUN MATERIAL * mtl_read(ACKFILE * file);

ACKFUN void mtl_write(ACKFILE * file, MATERIAL const * mtl);

ACKFUN void mtl_remove(MATERIAL * mtl);

// light api:
ACKFUN LIGHT * light_create(LIGHTTYPE type);

ACKFUN void light_remove(LIGHT * light);

ACKFUN LIGHT * light_next(LIGHT const * light);

#endif // _ACKNEXT_SCENE_H_
