#ifndef _ACKNEXT_FILESYS_H_
#define _ACKNEXT_FILESYS_H_

#include "config.h"
#include <stdint.h>

#include "blob.h"

// ACKFILE is opaque pointer to outer space, but
// inner space does have knowledge
typedef struct ackfile ACKFILE;

ACKFUN void filesys_addResource(char const * resource, char const * path);

ACKFUN ACKFILE * file_open_read(char const * name);

ACKFUN ACKFILE * file_open_write(char const * name);

ACKFUN ACKFILE * file_open_append(char const * name);

ACKFUN ACKFILE * file_open_blob(BLOB * blob, bool allowResize);

ACKFUN int64_t file_read(ACKFILE *file, void *buffer, uint32_t size);

ACKFUN int64_t file_write(ACKFILE *file, const void *buffer, uint32_t size);

ACKFUN void file_seek(ACKFILE * file, uint64_t position);

ACKFUN int64_t file_tell(ACKFILE * file);

ACKFUN int64_t file_size(ACKFILE * file);

ACKFUN bool file_eof(ACKFILE * file);

ACKFUN void file_flush(ACKFILE * file);

ACKFUN void file_close(ACKFILE * file);

#endif // _ACKNEXT_FILESYS_H_
