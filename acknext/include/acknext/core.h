#ifndef _ACKNEXT_CORE_H_
#define _ACKNEXT_CORE_H_

#include "config.h"
#include <stdbool.h>
#include <GL/gl3w.h>
#include "ackenum.h"

typedef enum ACKTYPE
{
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

ACKFUN int engine_main(void (*main)(), int argc, char ** argv);

ACKFUN bool engine_open(int argc, char ** argv);

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

ACKFUN void obj_setvar(void * obj, char const * name, GLenum type, ...);

ACKFUN void obj_listvar(void const * obj);

#endif // _ACKNEXT_CORE_H_
