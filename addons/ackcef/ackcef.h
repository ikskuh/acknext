#ifndef _ACKNEXT_EXT_ACKCEF_H_
#define _ACKNEXT_EXT_ACKCEF_H_

#include <acknext.h>

ACKFUN void ackcef_init(int argc, char ** argv);

ACKFUN VIEW * ackcef_createView();

ACKFUN void ackcef_navigate(VIEW * view, char const * url);

ACKFUN bool ackcef_ready(VIEW * view);

ACKFUN void ackcef_exec(VIEW * view, char const * js);

ACKFUN void ackcef_refresh(VIEW * view);

#endif // _ACKNEXT_EXT_ACKCEF_H_
