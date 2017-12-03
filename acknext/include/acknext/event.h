#ifndef _ACKNEXT_EVENT_H_
#define _ACKNEXT_EVENT_H_

#include "config.h"

typedef struct
{

} EVENT;

typedef void (*EVENTHANDLER)(void * arg);

ACKFUN EVENT * event_create();

ACKFUN void event_attach(EVENT ACKCONST * ev, EVENTHANDLER handler);

ACKFUN void event_detach(EVENT ACKCONST * ev, EVENTHANDLER handler);

ACKFUN void event_clear(EVENT ACKCONST * ev);

ACKFUN void event_invoke(EVENT ACKCONST * ev, void * arg);

ACKFUN void event_remove(EVENT * ev);

#endif
