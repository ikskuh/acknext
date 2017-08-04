#ifndef _ACKNEXT_CORE_H_
#define _ACKNEXT_CORE_H_

#include "config.h"
#include <stdbool.h>
#include "ackenum.h"

ACKFUN bool engine_open(int argc, char ** argv);

ACKFUN bool engine_frame();

ACKFUN void engine_close();

ACKFUN void engine_log(char const * format, ...);

ACKFUN void engine_seterror(ERROR code, char const * message, ...);

ACKVAR ERROR ACKCONST engine_lasterror;

ACKVAR char const * ACKCONST engine_lasterror_text;

#endif // _ACKNEXT_CORE_H_
