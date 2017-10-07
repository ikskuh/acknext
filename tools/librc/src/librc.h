#ifndef LIBRC_H
#define LIBRC_H

#include <stdlib.h>
#include <stdbool.h>

#ifdef STBI_MALLOC
#error Must include librc.h before any stb libraries!
#endif

#define STBI_MALLOC(sz)           malloc(sz)
#define STBI_REALLOC(p,newsz)     realloc(p,newsz)
#define STBI_FREE(p)              free(p)

#include "../include/acknext/librc.h"

#include <acknext/acff.h>
#include <acknext/serialization.h>
#include <acknext/extension.h>

#ifdef __cplusplus
#define C_API extern "C"
#else
#define C_API
#endif

extern rcLogCallback librc_callback;

#define LOG(...) do { if(librc_callback) librc_callback(__VA_ARGS__); } while(false)

#endif // LIBRC_H
