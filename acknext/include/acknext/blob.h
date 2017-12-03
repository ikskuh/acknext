#ifndef _ACKNEXT_BLOB_H
#define _ACKNEXT_BLOB_H

#include "config.h"
#include <stddef.h>

typedef struct
{
	size_t ACKCONST size;
	void * ACKCONST data;
} BLOB;

ACKFUN BLOB * blob_create(size_t size);

// Creates a BLOB view onto a given portion of memory
// TODO: Implement
ACKFUN BLOB * blob_createAlias(void * target, size_t size);

ACKFUN BLOB * blob_load(char const * fileName);

// TODO: Implement "blob_loadz" for decompressing a blob from a file stream
ACKFUN BLOB * blob_loadz(char const * fileName);

ACKFUN BLOB * blub_clone(BLOB const * blob);

// Inflates a blob compressed with gzip or zlib compression
ACKFUN BLOB * blob_inflate(BLOB const * blob);

// TODO: Implement compression of a blob
ACKFUN BLOB * blob_deflare(BLOB const * blob);

ACKFUN void blob_resize(BLOB * blob, size_t size);

ACKFUN void blob_save(BLOB const * blob, char const * fileName);

// TODO: Implement blob_savez for compressing a blob into a file stream
ACKFUN void blob_savez(BLOB const * blob, char const * fileName);

ACKFUN void blob_remove(BLOB * blob);

#endif // _ACKNEXT_BLOB_H
