#ifndef _ACKNEXT_EXT_ACKCEF_H_
#define _ACKNEXT_EXT_ACKCEF_H_

#include <acknext.h>

// Initialize ackcef BEFORE calling engine_open with
// the argc,argv values passed to main.
// NOTE: Don't use addons/ackmain with ackcef!
ACKFUN void ackcef_init(int argc, char ** argv);

// Creates a new browser view
ACKFUN VIEW * ackcef_createView();

// Navigates to a url in the view
ACKFUN void ackcef_navigate(VIEW * view, char const * url);

// Checks if the view has loaded
ACKFUN bool ackcef_ready(VIEW * view);

// Executes a piece of javascript
ACKFUN void ackcef_exec(VIEW * view, char const * js);

// Refreshes the current page
ACKFUN void ackcef_refresh(VIEW * view);

#endif // _ACKNEXT_EXT_ACKCEF_H_
