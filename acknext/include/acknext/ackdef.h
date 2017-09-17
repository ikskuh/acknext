#ifndef _ACKNEXT_ACKDEF_H_
#define _ACKNEXT_ACKDEF_H_

#include "config.h"
#include "ackmath.h"

#define USE_VFS (1<<0)
#define FULLSCREEN (1<<1)
#define DESKTOP (1<<2)
#define DIAGNOSTIC (1<<3)
#define CUSTOM_VIDEO (1<<4)
#define VFS_USE_CWD (1<<5)
#define SILENT_FAIL (1<<6)
#define VISIBLE (1<<0)
#define GLIDE (1<<0)
#define NOMOVE (1<<1)
#define TESSELATION (1<<0)
#define LINKED (1<<1)
#define CLEAR_DEPTH (1<<0)
#define CLEAR_COLOR (1<<1)
#define CLEAR_STENCIL (1<<2)

typedef BITFIELD CONFIGFLAGS;
typedef BITFIELD ENTITYFLAGS;
typedef BITFIELD MOVEFLAGS;
typedef BITFIELD SHADERFLAGS;
typedef BITFIELD STAGEFLAGS;
typedef BITFIELD VIEWFLAGS;
typedef BITFIELD WARPFLAGS;

#define CLEAR_TARGET (CLEAR_COLOR | CLEAR_DEPTH | CLEAR_STENCIL)

#endif // _ACKNEXT_ACKDEF_H_
