#ifndef _ACKNEXT_LIBRC_H_
#define _ACKNEXT_LIBRC_H_

#include <acknext.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*rcLogCallback)(char const * message, ...);

typedef struct rcImage
{
	GLenum target, textureFormat, pixelFormat, pixelType;
	size_t width, height, depth;
	size_t bufsiz;
	void * pixels;
} rcImage;

typedef struct rcMaterial
{
	COLOR albedo;
	COLOR emission;
	float roughness;
	float metalness;
	float fresnell;
	char const * albedoTexture;
	char const * normalTexture;
	char const * attributeTexture;
	char const * emissionTexture;

	bool embedTextures; // texture path will be loaded and embedded in librc_write_mtl
	bool shareTextures; // flag for modifying the texture symlink to share resources
} rcMaterial;


// used for linearizing LDR images.
extern float librc_image_gamma; // = 2.2

/******************************************************************************
 * Generic API                                                                *
 ******************************************************************************/

void librc_init(rcLogCallback callback);

void librc_close();

/******************************************************************************
 * Texture/Bitmap API                                                         *
 ******************************************************************************/

// Access ATX textures
void librc_write_image(ACKFILE * dest, rcImage const * image);

// read from "real" filesys
rcImage * librc_load_image(char const * fileName);

// deletes image->pixels, then image
void librc_free_image(rcImage * image);

/******************************************************************************
 * Material API                                                               *
 ******************************************************************************/

void librc_write_mtl(ACKFILE * dest, rcMaterial const * mtl);

// read from "real" filesys
rcMaterial * librc_load_mtl(char const * fileName);

// deletes all strings, then the mtl
void librc_free_mtl(rcMaterial * mtl);

#ifdef __cplusplus
}
#endif

#endif // _ACKNEXT_LIBRC_H_
