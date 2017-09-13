/**
 * This is not a core incude file!
 *
 * It's contents are not required by a normal user but are shared between
 * tools that are written with or for acknext.
 */
#ifndef _ACKNEXT_ACKTENSIOS_H_
#define _ACKNEXT_ACKTENSIOS_H_

#include "../acknext.h"

typedef struct EXTENSION
{
	ACKTYPE (*canLoad)(ACKGUID const * guid);

	MODEL*    (*loadModel)   (ACKFILE * file, ACKGUID const * guid);
	SHADER*   (*loadShader)  (ACKFILE * file, ACKGUID const * guid);
	MATERIAL* (*loadMaterial)(ACKFILE * file, ACKGUID const * guid);
	SOUND*    (*loadSound)   (ACKFILE * file, ACKGUID const * guid);
	// MUSIC* (*loadMusic)(ACKFILE * file, ACKGUID const * guid);
	HULL*     (*loadHull)    (ACKFILE * file, ACKGUID const * guid);
	BLOB*     (*loadBlob)    (ACKFILE * file, ACKGUID const * guid);
	BITMAP*   (*loadBitmap)  (ACKFILE * file, ACKGUID const * guid);
	BUFFER*   (*loadBuffer)  (ACKFILE * file, ACKGUID const * guid);
	MESH*     (*loadMesh)    (ACKFILE * file, ACKGUID const * guid);
	VIEW*     (*loadView)    (ACKFILE * file, ACKGUID const * guid);
	LIGHT*    (*loadLight)   (ACKFILE * file, ACKGUID const * guid);
} EXTENSION;

// extension MUST BE ALLOCATED BY CALLER and be available until
// the engine quits
ACKFUN bool ext_register(char const * name, EXTENSION * extension);

#endif // _ACKNEXT_ACKTENSIOS_H_
