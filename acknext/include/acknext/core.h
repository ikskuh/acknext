#ifndef _ACKNEXT_CORE_H_
#define _ACKNEXT_CORE_H_

#include "config.h"
#include <stdbool.h>
#include <GL/gl3w.h>
#include "ackenum.h"
#include "ackdef.h"

typedef enum ACKTYPE
{
	TYPE_SYMLINK = -1,
	TYPE_INVALID = 0,
	TYPE_MODEL = 1,
	TYPE_SHADER = 2,
	TYPE_MATERIAL = 3,
	TYPE_SOUND = 4,
	TYPE_MUSIC = 5,
	TYPE_HULL = 6,
	TYPE_BLOB = 7,
	TYPE_BITMAP = 8,
	TYPE_BUFFER = 9,
	TYPE_MESH = 10,
	TYPE_VIEW = 11,
	TYPE_LIGHT = 12,
} ACKTYPE;

typedef struct ACKGUID
{
	uint8_t id[16];
} __attribute__((packed)) ACKGUID;

typedef struct
{
	char const * argv0; // must be set by program main!

	char const * organization;
	char const * application;

	char const * windowTitle;
	SIZE resolution;

	CONFIGFLAGS flags;
} ACKCONFIG;

typedef struct
{
	int drawcalls;
} ENGINESTATS;

ACKVAR ACKCONFIG engine_config;

ACKVAR ENGINESTATS engine_stats;

ACKFUN int engine_main(void (*main)());

ACKFUN bool engine_open();

ACKFUN void engine_resize(int width, int height);

ACKFUN bool engine_frame();

ACKFUN void engine_close();

ACKFUN void engine_shutdown();

ACKFUN void engine_log(char const * format, ...);

ACKFUN void engine_seterror(ERROR code, char const * message, ...);

ACKVAR ERROR ACKCONST engine_lasterror;

ACKVAR char const * ACKCONST engine_lasterror_text;

ACKVAR char ACKCONST ** ACKCONST engine_argv;
ACKVAR int ACKCONST engine_argc;

#define mtl_setvar    obj_setvar
#define ent_setvar    obj_setvar
#define shader_setvar obj_setvar
#define bmap_setvar   obj_setvar
#define mesh_setvar   obj_setvar

#define ACK_POINTER   0xA001
// #define ACK_STRING    0xA002

ACKFUN void obj_setvar(void * obj, char const * name, GLenum type, ...);

ACKFUN void const * obj_getvar(void * obj, char const * name, GLenum * type);

ACKFUN void obj_listvar(void const * obj);

// TODO: Implement
ACKFUN ACKGUID guid_create();

ACKFUN ACKGUID guid_parse(char const * text);

ACKFUN bool guid_compare(ACKGUID const * a, ACKGUID const * b);

#endif // _ACKNEXT_CORE_H_
