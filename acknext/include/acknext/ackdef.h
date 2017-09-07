#ifndef _ACKNEXT_ACKDEF_H_
#define _ACKNEXT_ACKDEF_H_

#include "config.h"
#include "ackmath.h"

#define VISIBLE (1<<0)
#define GLIDE (1<<0)
#define NOMOVE (1<<1)
#define TESSELATION (1<<0)
#define LINKED (1<<1)
#define DOUBLESIDED (1<<2)
#define CLEAR_DEPTH (1<<0)
#define CLEAR_COLOR (1<<1)
#define CLEAR_STENCIL (1<<2)
#define FULLSCREEN (1<<1)

typedef BITFIELD ENTITYFLAGS;
typedef BITFIELD MOVEFLAGS;
typedef BITFIELD SHADERFLAGS;
typedef BITFIELD STAGEFLAGS;
typedef BITFIELD VIEWFLAGS;
typedef BITFIELD WARPFLAGS;

#define CLEAR_TARGET (CLEAR_COLOR | CLEAR_DEPTH | CLEAR_STENCIL)

#endif // _ACKNEXT_ACKDEF_H_
