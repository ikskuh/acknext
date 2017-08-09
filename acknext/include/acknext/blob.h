#ifndef _ACKNEXT_LOB_H
#define _ACKNEXT_BLOB_H

#include "config.h"
#include <stddef.h>

typedef struct
{
	size_t ACKCONST size;
	void * ACKCONST data;
} BLOB;

ACKFUN BLOB * blob_create(size_t size);

ACKFUN BLOB * blob_load(char const * fileName);

ACKFUN BLOB * blub_clone(BLOB const * blob);

ACKFUN void blob_resize(BLOB * blob, size_t size);

ACKFUN void blob_save(BLOB const * blob, char const * fileName);

ACKFUN void blob_remove(BLOB * blob);

#endif // _ACKNEXT_BLOB_H
