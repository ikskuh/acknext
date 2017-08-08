#ifndef _ACKNEXT_ACKDEF_H_
#define _ACKNEXT_ACKDEF_H_

#include "config.h"
#include "ackmath.h"

#define GLIDE (1<<0)
#define CLEAR_DEPTH (1<<0)
#define CLEAR_COLOR (1<<1)
#define CLEAR_STENCIL (1<<2)
#define FULLSCREEN (1<<0)
#define VISIBLE (1<<1)
#define NOTELEPORT (1<<0)

typedef BITFIELD MOVEFLAGS;
typedef BITFIELD STAGEFLAGS;
typedef BITFIELD VIEWFLAGS;
typedef BITFIELD WARPFLAGS;

#define CLEAR_TARGET (CLEAR_COLOR | CLEAR_DEPTH | CLEAR_STENCIL)

#endif // _ACKNEXT_ACKDEF_H_
