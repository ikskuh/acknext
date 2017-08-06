#ifndef _ACKNEXT_CORE_H_
#define _ACKNEXT_CORE_H_

#include "config.h"
#include <stdbool.h>
#include "ackenum.h"

ACKFUN int engine_main(void (*main)(), int argc, char ** argv);

ACKFUN bool engine_open(int argc, char ** argv);

ACKFUN bool engine_frame();

ACKFUN void engine_close();

ACKFUN void engine_shutdown();

ACKFUN void engine_log(char const * format, ...);

ACKFUN void engine_seterror(ERROR code, char const * message, ...);

ACKVAR ERROR ACKCONST engine_lasterror;

ACKVAR char const * ACKCONST engine_lasterror_text;

ACKVAR char ACKCONST ** ACKCONST engine_argv;
ACKVAR int ACKCONST engine_argc;

#endif // _ACKNEXT_CORE_H_
